#include "stdafx.h"
#include "akane-lex.h"
#include <numeric>
#include <functional>
using namespace std;
using namespace AkaneLang;
using namespace AkaneLex;

AkaneLex::LexicalAnalyzer::LexicalAnalyzer(std::istream * _streamPointer) : dfaPointers(), streamPointer(_streamPointer), output(), lsg(*streamPointer)
{
}

DFA<LetterString, SimpleState> &AkaneLex::LexicalAnalyzer::makeDFA(std::istream &is, const std::string &dfaName = string(""))
{
	// 获取字母表
	vector<LetterString> alphabet;
	map<LetterString, LetterString> tagifyMap;

	string alphabetHeader;
	is >> alphabetHeader;
	if (alphabetHeader != "alphabet")
	{
		throw AkaneInputValueException("此处没有 alphabet: %s", alphabetHeader);
	}

	int alphabetSize;
	is >> alphabetSize;

	for (int i = 0; i < alphabetSize; i++)
	{
		int isLetterClass;
		is >> isLetterClass;
		if (isLetterClass == 0)
		{
			// 非字母类
			string lStr;
			is >> lStr;
			LetterString currLetter(lStr);
			currLetter = LetterString::unescape(currLetter);
			alphabet.push_back(currLetter);
		}
		else if (isLetterClass == 1)
		{
			// 字母类
			string lClassMarkStr;
			is >> lClassMarkStr;
			LetterString currLetterClassMark(lClassMarkStr);
			alphabet.push_back(currLetterClassMark);

			int letterClassSize;
			is >> letterClassSize;
			
			for (int j = 0; j < letterClassSize; j++)
			{
				string lStr;
				is >> lStr;
				LetterString currLetter(lStr);
				currLetter = LetterString::unescape(currLetter);
				tagifyMap[currLetter] = currLetterClassMark;
			}
		}
		else throw AkaneInputValueException("字母类标记只能为 1 / 0, 检测到 %d", isLetterClass);
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

	vector< SimpleState< LetterString>> states;
	
	vector<LetterString> stashedLetters;
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

		SimpleState<LetterString> currState;
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
				stashedLetters.push_back(LetterString(LetterString::epsilon()));
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

					currLetterStr = LetterString::unescape(currLetterStr);

					stashedLetters.push_back(LetterString());

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
	

	// 解释在 delta 函数中以 currLetterStr 指代目的状态的 delta 项

	for (int i = 0; i < alphabetSize; i++)
	{
		auto &l = alphabet[i];
		string letterVal = l.getValue();
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

		LetterString &currLetter = stashedLetters[stashedLetterIndex];
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
	NFA<LetterString, SimpleState> nfa(alphabet, tagifyMap, &this->lsg);
	nfa.loadStates(states);
	nfa.setInitialStateIndex(initialStateIndex);

	nfa.print(logger);

	// 造 DFA
	auto &dfa = *new DFA<LetterString, SimpleState>(nfa);
	dfa.validate();
	this->dfaPointers.push_back(&dfa);

	if (dfaName != "")
	{
		dfa.name = dfaName;
	}
	dfa.print(logger);

	return dfa;
}

void AkaneLex::LexicalAnalyzer::analyze()
{
	static ifstream f1("preprocessor-nfa.txt", ios::in);
	static auto &dfa_pp = makeDFA(f1, "预处理指令-词法分析 DFA");
	static ifstream f2("whitespaces-nfa.txt", ios::in);
	static auto &dfa_ws = makeDFA(f2, "空白字符-词法分析 DFA");
	static ifstream f3("keywordAndIdentifier-nfa.txt", ios::in);
	static auto &dfa_word = makeDFA(f3, "关键字/标识符-词法分析 DFA");
	static ifstream f4("integer-nfa.txt", ios::in);
	static auto &dfa_int = makeDFA(f4, "整数-词法分析 DFA");
	static ifstream f5("float-nfa.txt", ios::in);
	static auto &dfa_float = makeDFA(f5, "浮点-词法分析 DFA");
	
	size_t dfaNum = dfaPointers.size();

	vector<AutomataStateTag> prevDFATag(dfaNum, AutomataStateTag::notAccepted);
	vector<AutomataStateTag> currDFATag(dfaNum, AutomataStateTag::notAccepted);
	
	for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
	{
		auto &dfa = *dfaPointers[dfaI];
		dfa.reset();
	}

	bool everAccepted = false;

	while (true)
	try
	{
		Index someAcceptedDFAIndex = (numeric_limits<Index>::max)();
		for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
		{
			auto &dfa = *dfaPointers[dfaI];
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

			if (prevDFATag[dfaI] == AutomataStateTag::accepted && someAcceptedDFAIndex == (numeric_limits<Index>::max)())
			{
				someAcceptedDFAIndex = dfaI;
			}
		}

		bool allNotAccepted = true;
		for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
		{
			if (currDFATag[dfaI] != AutomataStateTag::notAccepted)
			{
				allNotAccepted = false;
				break;
			}
		}

		if (everAccepted && allNotAccepted)
		{
			// 从某一个 DFA 仍接受, 转变为没有 DFA 接受, 标志着一个词的结束
			string currWord;
			lsg.dumpCurrWord(currWord);
			
			logger << "识别 Token: ";
			logger << currWord << endl;
			logger << "到自动机: " << dfaPointers[someAcceptedDFAIndex]->name << endl << endl;
			everAccepted = false;
			for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
			{
				auto &dfa = *dfaPointers[dfaI];
				dfa.reset();
			}
		}
		else
		{
			GET_ASSIGN_AND_FREE(const LetterString, l1, lsg.peek_freeNeeded())
				;
			logger << "读: " << l1.getDescription() << endl;
			prevDFATag = currDFATag;
			for (Index dfaI = 0; dfaI < dfaNum; dfaI++)
			{
				auto &dfa = *dfaPointers[dfaI];
				logger << "DFA " << dfa.name << " 从状态 <" << dfa.states[dfa.getCurrStateIndex()].description << "> 跳转到 ";
				dfa.oneStep_peek();
				logger << "<" << dfa.states[dfa.getCurrStateIndex()].description << ">" << endl;
			}
			GET_ASSIGN_AND_FREE(const LetterString, l2, lsg.next_freeNeeded())
				;
			logger << "取出: " << l2.getDescription() << endl;
		}
	}
	catch (AkaneGetEOFException)
	{
		break;
	}

	string checkNotOnlyWhite;
	string firstChars;
	firstChars = lsg.currWord.str().substr(0, 40);
	lsg.currWord >> checkNotOnlyWhite;
	if (checkNotOnlyWhite != "")
	{
		throw AkaneLexAnalyzeException("从此处开始无法识别合法的词: %s", firstChars.c_str());
	}
}

std::vector<Token> AkaneLex::LexicalAnalyzer::getOutput()
{
	return std::vector<Token>();
}

AkaneLex::LexicalAnalyzer::~LexicalAnalyzer()
{
	for (auto p : dfaPointers)
	{
		delete p;
	}
}
