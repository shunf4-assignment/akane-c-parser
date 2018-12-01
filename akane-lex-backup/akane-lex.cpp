// akane-lex.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "akane-lex.h"
using namespace std;
using namespace AkaneLang;
using namespace AkaneLex;

AkaneLex::LexicalAnalyzer::LexicalAnalyzer(std::istream * _streamPointer) : dfaPointers(), streamPointer(_streamPointer), output(), lsg(*streamPointer)
{
}

string translate(const string &orig)
{
	if (orig == "[space]")
	{
		return " ";
	}
	if (orig == "[newline]")
	{
		return ("\n");
	}
	if (orig == "[tab]")
	{
		return ("\t");
	}
	if (orig == "[vtab]")
	{
		return ("\v");
	}
	return orig;
}

LetterString translate(const LetterString &orig)
{
	return LetterString(translate(orig.getText()));
}

DFA<LetterString, SimpleState> &AkaneLex::LexicalAnalyzer::makeDFA(std::istream &is)
{
	// 获取字母表
	vector<LetterString> alphabet;
	map<LetterString, LetterString> unescapeMap;

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
			currLetter = translate(currLetter);
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
				currLetter = translate(currLetter);
				unescapeMap[currLetter] = currLetterClassMark;
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
	vector<int> stashedStateIndices;
	map<string, vector<int>> namedLetters;
	map<string, vector<int>> namedStates;

	vector<tuple<int, int, int>> deltaEntries;
	
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
		currState.tag = accepted ? AutomataStateTag::accepted : AutomataStateTag::notAccepted;

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

			int stashedLettersIndex = stashedLetters.size();

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

					currLetterStr = translate(currLetterStr);

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

			int stashedStateIndicesIndex = stashedStateIndices.size();

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

				deltaEntries.push_back(tuple<int, int, int>(i, stashedLettersIndex, stashedStateIndicesIndex));
			}
		}
		/*
		for (int j = 0; j <= alphabetSize; j++)
		{
			int destStateSize;
			LetterString currLetter("");
			if (j == alphabetSize)
			{
				currLetter = LetterString::epsilon();
			}
			else
			{
				currLetter = alphabet[j];
			}
			// << "当前状态: " << i << endl;;
			//cout << "当前字母: " << currLetter.getText() << endl;;
			is >> destStateSize;
			if (destStateSize > 0)
			{
				currState.delta[currLetter] = std::set<int>{};
				for (int k = 0; k < destStateSize; k++)
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
						
						if (namedStates.count(stateName) == 0)
						{
							namedStates[stateName] = vector<pair<int, LetterString>>{};
						}
						namedStates[stateName].push_back(pair<int, LetterString>(i, currLetter));
					}
					else
					{
						currState.delta[currLetter].insert(si);
					}
					
				}
			}
		}*/

		states.push_back(currState);
	}
	

	// 解释在 delta 函数中以 currLetterStr 指代目的状态的 delta 项

	for (int i = 0; i < alphabetSize; i++)
	{
		auto &l = alphabet[i];
		string letterVal = l.getText();
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
		int stateIndex = get<0>(t);
		auto &d = states[stateIndex].delta;
		int stashedLetterIndex = get<1>(t);
		int stashedStateIndicesIndex = get<2>(t);

		LetterString &currLetter = stashedLetters[stashedLetterIndex];
		int &currDestStateIndex = stashedStateIndices[stashedStateIndicesIndex];

		if (d.count(currLetter) == 0)
		{
			d[currLetter] = set<int>{};
		}

		d[currLetter].insert(currDestStateIndex);
	}

	// 获取初始状态

	int initialStateIndex;
	is >> initialStateIndex;

	// 造 NFA
	NFA<LetterString, SimpleState> nfa(alphabet, unescapeMap, &this->lsg);
	nfa.loadStates(states);
	nfa.setInitialStateIndex(initialStateIndex);

	nfa.print();

	// 造 DFA
	auto &dfa = *new DFA<LetterString, SimpleState>(nfa);
	dfa.print();
	dfa.validate();
	this->dfaPointers.push_back(&dfa);

	return dfa;
}

void AkaneLex::LexicalAnalyzer::analyze()
{
	ifstream f("preprocessor-nfa.txt", ios::in);
	auto dfa = makeDFA(f);
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
