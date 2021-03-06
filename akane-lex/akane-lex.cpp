#include "stdafx.h"
#include "akane-lex.h"
#include <numeric>
#include <functional>
#include <iostream>

using namespace std;
using namespace AkaneLang;
using namespace AkaneLex;

// trim from start (in place)

void AkaneLex::Evaluator::ltrim(string & s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) {
		return !isspace(ch);
	}));
}

// trim from end (in place)

void AkaneLex::Evaluator::rtrim(string & s) {
	s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
		return !isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)

void AkaneLex::Evaluator::trim(string & s) {
	ltrim(s);
	rtrim(s);
}


TokenizedLetter AkaneLex::NullEvaluator::evaluate(istringstream & stream) const
{
	stream.seekg(0, ios::end);
	return TokenizedLetter::epsilon();
}
/*
template<class AutomataState>
inline LexDFA<AutomataState>::LexDFA(const vector<StringifiedLetter> &_alphabet, const map<StringifiedLetter, StringifiedLetter> &_tagifyMap, AkaneLang::LetterGenerator *_letterGeneratorP, Evaluator *_evaluatorP)
	: AkaneLang::DFA<L, AutomataState>(_alphabet, _tagifyMap, _letterGeneratorP)
{
	this->AkaneLang::DFA<L, AutomataState>::DFA(_alphabet, _tagifyMap, _letterGeneratorP);
}
*/


template<class AutomataState>
inline AkaneLex::LexDFA<AutomataState>::LexDFA(AkaneLang::NFA<AutomataState> &nfa, const Evaluator *_evaluatorP)
	: AkaneLang::DFA<AutomataState>(nfa), evaluatorP(_evaluatorP)
{
}

template<class AutomataState>
AkaneLex::LexDFA<AutomataState>::LexDFA(istream & is, StreamedLetterGenerator *stgP, const Evaluator *_evaluatorP)
	: evaluatorP(_evaluatorP)
{
	is >> ws;
	string nfaName;
	getline(is, nfaName);

	// 获取字母表
	vector<StringifiedLetter> alphabet;
	map<StringifiedLetter, StringifiedLetter> tagifyMap;

	string alphabetHeader;
	is >> alphabetHeader;
	if (alphabetHeader != "alphabet")
	{
		throw AkaneInputValueException("此处没有 alphabet: %s", alphabetHeader);
	}

	int alphabetSize;
	is >> alphabetSize;
	bool enabledElse = false;

	for (int i = 0; i < alphabetSize; i++)
	{
		int letterClassFlag;
		is >> letterClassFlag;
		if (letterClassFlag == 0)
		{
			// 非字母类
			string lStr;
			is >> lStr;
			StringifiedLetter currLetter(lStr);
			currLetter = unescape(currLetter);
			alphabet.push_back(currLetter);
		}
		else if (letterClassFlag == 1)
		{
			// 字母类
			string lClassMarkStr;
			is >> lClassMarkStr;
			StringifiedLetter currLetterClassMark(lClassMarkStr);
			alphabet.push_back(currLetterClassMark);

			int letterClassSize;
			is >> letterClassSize;

			for (int j = 0; j < letterClassSize; j++)
			{
				string lStr;
				is >> lStr;
				StringifiedLetter currLetter(lStr);
				currLetter = unescape(currLetter);
				tagifyMap[currLetter] = currLetterClassMark;
			}
		}
		else if (letterClassFlag == 2)
		{
			// else 类
			string lClassMarkStr;
			is >> lClassMarkStr;
			if (lClassMarkStr != "[else]")
			{
				throw AkaneInputValueException("else 类标记只能为 [else], 检测到 %s", lClassMarkStr);
			}
			StringifiedLetter currLetterClassMark(lClassMarkStr);
			alphabet.push_back(currLetterClassMark);

			enabledElse = 1;
		}
		else throw AkaneInputValueException("字母类标志只能为 2 / 1 / 0, 检测到 %d", letterClassFlag);
	}

	// 获取状态

	string statesHeader;
	is >> statesHeader;
	if (statesHeader != "states")
	{
		throw AkaneInputValueException("此处没有 states: %s", statesHeader);
	}


	int statesNum;
	is >> statesNum;

	vector<BaseState> states;

	vector<StringifiedLetter> stashedLetters;
	vector<Index> stashedStateIndices;
	map<string, vector<Index>> namedLetters;
	map<string, vector<Index>> namedStates;

	vector<tuple<StateIndex, Index, StateIndex>> deltaEntries;

	for (int i = 0; i < statesNum; i++)
	{
		int no;
		is >> no;
		if (no != i)
		{
			throw AkaneInputValueException("状态的序号只能为 %d, 检测到 %d", i, no);
		}
		string currStr;
		is >> currStr;
		int accepted;
		is >> accepted;

		BaseState currState;
		currState.description = currStr;
		currState.tag = (AutomataStateTag)accepted;

		bool first_epsilon = true;

		// 读取可转移字母和目标状态
		while (true)
		{
			int currLetterIndex;
			int destStatesSize;
			string currLetterStr;

			is >> destStatesSize;
			if (destStatesSize < 0)
			{
				break;
			}

			Index stashedLettersIndex = stashedLetters.size();

			if (first_epsilon)
			{
				is >> currLetterStr;
				stashedLetters.push_back(StringifiedLetter(StringifiedLetter::epsilon()));
				first_epsilon = false;
			}
			else
			{
				is >> currLetterIndex;

				if (is.fail())
				{
					is.clear();
					is >> currLetterStr;
					if (currLetterStr.front() != '<' || currLetterStr.back() != '>')
					{
						throw AkaneInputValueException("字母名/字母类标记出错 : %s", currLetterStr);
					}
					currLetterStr.erase(0, 1);
					currLetterStr.pop_back();

					currLetterStr = unescape(currLetterStr);

					stashedLetters.push_back(StringifiedLetter());

					if (namedLetters.count(currLetterStr) == 0)
					{
						//namedLetters[currLetterStr] = vector<int>{};
					}

					namedLetters[currLetterStr].push_back(stashedLettersIndex);
				}
				else
				{
					stashedLetters.push_back(alphabet[currLetterIndex]);
				}
			}

			Index stashedStateIndicesIndex = stashedStateIndices.size();

			for (int k = 0; k < destStatesSize; k++)
			{
				int si;
				is >> si;

				if (is.fail())
				{
					is.clear();
					string stateName;
					is >> stateName;

					if (stateName.front() != '<' || stateName.back() != '>')
					{
						throw AkaneInputValueException("状态下标/状态名出错 : %s", stateName);
					}
					stateName.erase(0, 1);
					stateName.pop_back();

					stashedStateIndices.push_back(-1);

					if (namedStates.count(stateName) == 0)
					{
						//namedStates[stateName] = vector<int>{};
					}

					namedStates[stateName].push_back(stashedStateIndicesIndex);
				}
				else
				{
					stashedStateIndices.push_back(si);
				}

				deltaEntries.push_back(tuple<StateIndex, Index, StateIndex>(i, stashedLettersIndex, stashedStateIndicesIndex));
			}
		}
		states.push_back(currState);
	}


	// 解释在 delta 函数中以 currLetterStr 指代转移符号的 delta 项

	for (int i = 0; i < alphabetSize; i++)
	{
		auto &l = alphabet[i];
		string letterVal = l.getUniqueName();
		if (namedLetters.count(letterVal) == 1)
		{
			for (auto li : namedLetters[letterVal])
			{
				stashedLetters[li] = l;
			}
		}
	}

	// 解释在 delta 函数中以 stateName 指代目的状态的 delta 项

	for (int i = 0; i < statesNum; i++)
	{
		auto &s = states[i];
		string stateDesc = s.description;
		if (namedStates.count(stateDesc) == 1)
		{
			for (auto si : namedStates[stateDesc])
			{
				stashedStateIndices[si] = i;
			}
		}
	}

	// 将 deltaEntries 中的各下标逐一去除解析, 输入到各 state 的 delta 中.

	for (auto &t : deltaEntries)
	{
		StateIndex stateIndex = get<0>(t);
		auto &d = states[stateIndex].delta;
		Index stashedLetterIndex = get<1>(t);
		StateIndex stashedStateIndicesIndex = get<2>(t);

		StringifiedLetter &currLetter = stashedLetters[stashedLetterIndex];
		StateIndex &currDestStateIndex = stashedStateIndices[stashedStateIndicesIndex];

		if (d.count(currLetter) == 0)
		{
			d[currLetter] = set<StateIndex>{};
		}

		d[currLetter].insert(currDestStateIndex);
	}

	// 获取初始状态

	int initialStateIndex;
	is >> initialStateIndex;

	// 造 NFA
	NFA<BaseState> nfa(alphabet, states, tagifyMap, stgP, nfaName, initialStateIndex);
	nfa.isNotMatchedLettersMappedToElse = enabledElse;

#ifdef _DEBUG
	nfa.print(logger);
#endif

	// 造 DFA
	this->DFA<BaseState>::DFA(nfa);

#ifdef _DEBUG
	this->print(logger);
#endif
}

template<class AutomataState>
inline string AkaneLex::LexDFA<AutomataState>::typeStr() const
{
	return "Lexical DFA";
}

AkaneLex::BaseLexicalAnalyzer::BaseLexicalAnalyzer(StreamedLetterGenerator & _inputStreamedLetterGenerator)
	: inputStreamedLetterGenerator(_inputStreamedLetterGenerator), output(), outputTokenizedLetterGenerator(output)
{
}

void AkaneLex::BaseLexicalAnalyzer::analyze()
{
	size_t dfaNum = dfas.size();

	vector<AutomataStateTag> prevDFATag(dfaNum, AutomataStateTag::notAccepted);
	vector<AutomataStateTag> currDFATag(dfaNum, AutomataStateTag::notAccepted);

	for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
	{
		auto &dfa = dfas[dfaI];
		dfa.reset();
	}

	bool everAccepted = false;

	while (true)
		try
	{
		Index someAcceptedDFAIndex = (numeric_limits<Index>::max)();
		bool prevAccepted = false;
		for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
		{
			auto &dfa = dfas[dfaI];
			try
			{
				currDFATag[dfaI] = dfa.peekStateTag();
			}
			catch (AkaneGetEOFException)
			{
				currDFATag[dfaI] = AutomataStateTag::notAccepted;
			}

			if (currDFATag[dfaI] == AutomataStateTag::accepted)
			{
				everAccepted = true;
			}

			if (prevDFATag[dfaI] == AutomataStateTag::accepted)
			{
				prevAccepted = true;
				if (someAcceptedDFAIndex == (numeric_limits<Index>::max)())
					someAcceptedDFAIndex = dfaI;
			}
		}

		bool allRefused = true;
		for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
		{
			if (currDFATag[dfaI] != AutomataStateTag::notAccepted)
			{
				allRefused = false;
				break;
			}
		}

		if (everAccepted && allRefused)
		{
			if (!prevAccepted)
			{
				// 从某几个 DFA 中间状态, 其他 DFA 拒绝 跳转到 所有 DFA 拒绝, 识别失败
				throw AkaneLexAnalyzeException("从此处开始无法识别合法的词: %s", inputStreamedLetterGenerator.currLexemeStream.str().substr(0, 40).c_str());
			}

			// 从某一个 DFA 仍接受, 转变为没有 DFA 接受, 标志着一个词的结束
			string currLexeme;
			inputStreamedLetterGenerator.dumpCurrLexeme(currLexeme);

			istringstream is;
			is.str(currLexeme);

			TokenizedLetter tl = dfas[someAcceptedDFAIndex].evaluatorP->evaluate(is);

			if (tl != TokenizedLetter::epsilon())
				this->output.push_back(tl);

			logger << "识别字符串: ";
			logger << currLexeme << " 为 Token : ";
			logger << tl.getLongDescription();
			logger << " 到自动机: " << dfas[someAcceptedDFAIndex].name << endl << endl;

			everAccepted = false;
			for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
			{
				auto &dfa = dfas[dfaI];
				dfa.reset();
			}
		}
		else
		{
			GET_ASSIGN_AND_FREE(StringifiedLetter, l1, *inputStreamedLetterGenerator.peek_freeNeeded());

			logger << "读: " << l1.getShortDescription() << endl;
			prevDFATag = currDFATag;
			for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
			{
				auto &dfa = dfas[dfaI];
				logger << "DFA " << dfa.name << " 从状态 <" << dfa.states[dfa.currStateIndex].description << "> 跳转到 ";
				dfa.goOneStep_peek();
				logger << "<" << dfa.states[dfa.currStateIndex].description << ">" << endl;
			}

			inputStreamedLetterGenerator.ignoreOne();
		}
	}
	catch (AkaneGetEOFException)
	{
		break;
	}

	string checkNotOnlyWhite;
	string firstChars;
	firstChars = inputStreamedLetterGenerator.currLexemeStream.str().substr(0, 40);
	inputStreamedLetterGenerator.currLexemeStream >> checkNotOnlyWhite;
	if (checkNotOnlyWhite != "")
	{
		throw AkaneLexAnalyzeException("从此处开始无法识别合法的词: %s", firstChars.c_str());
	}

	bool backup = logger.printLogToStdout;
	logger.printLogToStdout = true;
	logger << "词法分析后的 Token 列表: " << endl;
	for (auto &tl : output)
	{
		logger << tl.getLongDescription() << endl;
	}
	logger.printLogToStdout = backup;
}

TokenizedLetterGenerator & AkaneLex::BaseLexicalAnalyzer::getOutput()
{
	return outputTokenizedLetterGenerator;
}

AkaneLex::BaseLexicalAnalyzer::~BaseLexicalAnalyzer()
{
}

template struct AkaneLex::LexDFA<BaseState>;