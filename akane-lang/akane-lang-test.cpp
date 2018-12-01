#include "akane-lang.h"
#include <iostream>
#include <fstream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::vector;
using std::string;
using std::set;
#include <cassert>
using namespace AkaneLang;

int main()
{
	std::ifstream f("test-nfa.txt", std::ios::in);
	if (f.is_open() == false)
	{
		throw AkaneSystemException("文件不存在");
	}

	vector<LetterString> alphabet;

	NFA<LetterString, SimpleState> nfa;

	int alphabetLen;
	f >> alphabetLen;

	for (int i = 0; i < alphabetLen; i++)
	{
		string currStr;
		f >> currStr;
		LetterString ls(currStr);
		cout << "字母 : " << currStr << endl;
		alphabet.push_back(ls);
	}

	int statesLen;
	f >> statesLen;

	vector< SimpleState< LetterString>> states;
	
	for (int i = 0; i < statesLen; i++)
	{
		int testI;
		f >> testI;
		assert(testI == i);
		string currStr;
		f >> currStr;
		int accepted;
		f >> accepted;

		SimpleState< LetterString> currState;
		currState.description = currStr;
		currState.tag = (AutomataStateTag)accepted;

		for (int j = 0; j <= alphabetLen; j++)
		{
			int destStateLen;
			LetterString currLetter("");
			if (j == alphabetLen)
			{
				currLetter = LetterString::epsilon();
				
			}
			else
			{
				currLetter = alphabet[j];
			}
			cout << "当前状态: " << i << endl;;
			cout << "当前字母: " << currLetter.getValue() << endl;;
			f >> destStateLen;
			if (destStateLen > 0)
			{
				currState.delta[currLetter] = std::set<StateIndex>{};
				for (int k = 0; k < destStateLen; k++)
				{
					int si;
					f >> si;
					cout << si << endl;
					currState.delta[currLetter].insert(si);
				}
			}
		}

		states.push_back(currState);
	}

	int initialStateIndex;
	f >> initialStateIndex;

	nfa.loadAlphabet(alphabet);
	nfa.loadStates(states);
	nfa.setInitialStateIndex(initialStateIndex);

	nfa.print<AkaneUtils::Logger>(logger);

	DFA<LetterString, SimpleState> dfa(nfa);
	dfa.print<AkaneUtils::Logger>(logger);
	dfa.validate();
	
	return 0;
}