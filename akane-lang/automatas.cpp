#include "stdafx.h"
#include "akane-lang.h"
#include <memory>

using namespace std;
namespace AkaneLang
{
	const char *automataStateTagText[] =
	{
		"�ܾ�",
		"����",
		"�м�"
	};

	const StateIndex invalidStateIndex = std::numeric_limits<StateIndex>::max();

	template <class AutomataState>
	AkaneLang::NFA<AutomataState>::NFA() : letterGeneratorP((static_cast<LetterGenerator *>(nullptr)))
	{
		name = "(δ����)";
		initialStateIndex = invalidStateIndex;
		isNotMatchedLettersMappedToElse = false;
	}

	template <class AutomataState>
	AkaneLang::NFA<AutomataState>::NFA(const std::vector<StringifiedLetter> &_alphabet, const std::vector<AutomataState> &_states, const std::map<StringifiedLetter, StringifiedLetter> &_tagifyMap, LetterGenerator *_letterGeneratorP, const std::string &_name, StateIndex _initialStateIndex) : alphabet(_alphabet), states(_states), tagifyMap(_tagifyMap), letterGeneratorP(_letterGeneratorP), name(_name), initialStateIndex(_initialStateIndex)
	{
		isNotMatchedLettersMappedToElse = false;
	}

	template <class AutomataState>
	std::string AkaneLang::NFA<AutomataState>::typeStr() const
	{
		return std::string("NFA");
	}
/*
	template <class AutomataState>
	void AkaneLang::NFA<AutomataState>::addState(AutomataState<L> state)
	{
		states.push_back(state);
	}

	template <class AutomataState>
	void AkaneLang::NFA<AutomataState>::loadAlphabet(const std::vector<L>& _alphabet)
	{
		alphabet = _alphabet;
	}

	template <class AutomataState>
	void NFA<AutomataState>::loadtagifyMap(const std::map<L, L>& _tagifyMap)
	{
		tagifyMap = _tagifyMap;
	}

	template <class AutomataState>
	void AkaneLang::NFA<AutomataState>::loadStates(const std::vector<AutomataState<L>>& _states)
	{
		states.clear();
		states.insert(states.end(), _states.begin(), _states.end());
	}

	template <class AutomataState>
	void AkaneLang::NFA<AutomataState>::loadDeltas(std::vector<std::map<L, std::set<StateIndex>>>& _indexedDeltas)
	{
		if (_indexedDeltas.size() != states.size())
		{
			throw AkaneLangException("_indexedDeltas ��С������");
		}

		auto d = _indexedDeltas.begin();
		auto s = states.begin();

		for (; d != _indexedDeltas.end(); d++, s++)
		{
			auto dR = *d;
			s->delta.clear();
			for (auto &dPair : dR)
			{
				s->delta.insert(std::pair<L, std::set< StateIndex>>(dPair.first, dPair.second));
			}
		}
	}

	template<class AutomataState>
	inline void NFA<AutomataState>::setInitialStateIndex(size_t _initialStateIndex) { initialStateIndex = _initialStateIndex; }

	template<class AutomataState>
	inline size_t NFA<AutomataState>::getInitialStateIndex() const { return initialStateIndex; }
*/

	template<class AutomataState>
	inline void NFA<AutomataState>::mergeByStateIndices(AutomataState & dest, const std::set<StateIndex>& srcIndices) const
	{
		dest.startMerge();
		for (auto si : srcIndices)
		{
			dest.mergeThisState(states[si]);
		}
		dest.endMerge();
	}

	template <class AutomataState>
	std::set<StateIndex> NFA<AutomataState>::getEpsilonClosureOf(const std::set<StateIndex>& sis) const
	{
		std::set<StateIndex> result;
		for (auto si : sis)
		{
			result.insert(si);
		}

		size_t prevSize = result.size();
		while (true)
		{
			// ����ʱ�����п�?
			for (auto si : result)
			{
				if (states[si].delta.count(StringifiedLetter::epsilon()) == 1)
				{
					auto &epsilonSet = states[si].delta.at(StringifiedLetter::epsilon());
					result.insert(epsilonSet.begin(), epsilonSet.end());
				}
			}
			if (result.size() == prevSize)
				break;
			prevSize = result.size();
		}
		return result;
	}

	template <class AutomataState>
	template <class Out>
	void AkaneLang::NFA<AutomataState>::print(Out &out) const
	{
		using namespace std;
		out << "==== FA ��ӡ ====" << endl;
		out << "����: " << name << " (" << hex << this << dec << ", " << this->typeStr() << ")" << endl;
		out << "��ĸ��: " << (isNotMatchedLettersMappedToElse ? " ([else] ����)" : "") << endl;
		for (auto &l : alphabet)
		{
			out << l.getShortDescription();
			bool first = true;
			for (auto &p : tagifyMap)
			{
				if (p.second == l)
				{
					if (first)
					{
						first = false;
						out << " : ";
					}
					
					out << p.first.getShortDescription() << " ";
				}
			}
			out << endl;
		}
		out << endl;

		out << "��ʼ״̬���: " << initialStateIndex << endl;

		for (unsigned i = 0; i < states.size(); i++)
		{
			out << "״̬" << i << " : <" << states[i].description << "> " << automataStateTagText[states[i].tag] << ". ";
			out << "ת�ƺ��� : " << endl;

			bool empty = true;
			for (auto &dPair : states[i].delta)
			{
				if (empty)
					empty = false;
				out << dPair.first.getShortDescription() << " -> ";
				if (dPair.second.size() > 0) {
					for (auto &destStateIndex : dPair.second)
					{
						out << destStateIndex << " <" << states[destStateIndex].description << ">;";
					}
					out << endl;
				}
				else out << "(��)  ";
			}
			if (empty)
			{
				out << "(��)";
			}

			out << endl;
		}
		out << "�Ƿ�ͨ�����?" << endl;
		try
		{
			validate();
			out << "��" << endl;
		}
		catch (AkaneLangException)
		{
			out << "��" << endl;
		}
		out << endl;
	}

	template <class AutomataState>
	void AkaneLang::NFA<AutomataState>::validate() const
	{
		;
	}
/*
	template <class AutomataState>
	inline std::string AkaneLang::NFA<AutomataState>::makeDescription(std::set<StateIndex>& _stateSet)
	{
		std::string newDesc;
		for (auto si : _stateSet)
		{
			newDesc += states[si].description;
			newDesc += ",";
		}
		if (newDesc.length() == 0)
		{
			newDesc = "(empty)";
		}
		else newDesc.pop_back();
		return newDesc;
	}
*/
	template <class AutomataState>
	inline bool AkaneLang::NFA<AutomataState>::haveAcceptedStateIn(const std::set<StateIndex>& _stateSet) const
	{
		bool haveAcceptedStateIn = false;
		for (auto si : _stateSet)
		{
			if (states[si].tag == AutomataStateTag::accepted)
			{
				haveAcceptedStateIn = true;
				break;
			}
		}

		return haveAcceptedStateIn;
	}

	template <class AutomataState>
	inline bool AkaneLang::NFA<AutomataState>::haveIntermediateStateIn(const std::set<StateIndex>& _stateSet) const
	{
		bool haveIntermediateStateIn = false;
		for (auto si : _stateSet)
		{
			if (states[si].tag == AutomataStateTag::intermediate)
			{
				haveIntermediateStateIn = true;
				break;
			}
		}

		return haveIntermediateStateIn;
	}

	/*
	template <class AutomataState>
	AkaneLang::DFA<AutomataState>::DFA(const std::vector<L>& _alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator * _letterGeneratorP) : AkaneLang::NFA<AutomataState>::NFA(_alphabet, _tagifyMap, _letterGeneratorP)
	{
		currStateIndex = invalidStateIndex;
	}
	*/

	template<class AutomataState>
	DFA<AutomataState>::DFA() : NFA<AutomataState>()
	{
		currStateIndex = invalidStateIndex;
	}

	template <class AutomataState>
	AkaneLang::DFA<AutomataState>::DFA(NFA<AutomataState>& nfa) : NFA<AutomataState>(nfa.alphabet, { }, nfa.tagifyMap, nfa.letterGeneratorP, "(NotInitialized)", invalidStateIndex), currStateIndex(invalidStateIndex)
	{
		this->isNotMatchedLettersMappedToElse = nfa.isNotMatchedLettersMappedToElse;

		lg("==== ��ʼ�� NFA %s תΪ DFA %s ====", nfa.name.c_str(), NFA<AutomataState>::name.c_str());
		std::vector< std::set< StateIndex>> statesInSetForm;
		this->states.clear();
		// ������һ��״̬
		AutomataState first;
		std::set <StateIndex> tmpFirst = std::set< StateIndex>{ nfa.initialStateIndex };
		lg("��һ��״̬: %d", nfa.initialStateIndex);
		std::set <StateIndex> trueFirstIndices = nfa.getEpsilonClosureOf(tmpFirst);
		lg("��һ��״̬�հ���: ");
		for (auto si : trueFirstIndices)
		{
			lg("%d", si);
		}

		//first.description = nfa.makeDescription(trueFirstIndices);
		nfa.mergeByStateIndices(first, trueFirstIndices);
		first.tag = nfa.haveAcceptedStateIn(trueFirstIndices) ? AutomataStateTag::accepted : nfa.haveIntermediateStateIn(trueFirstIndices) ? AutomataStateTag::intermediate : AutomataStateTag::notAccepted;

		NFA<AutomataState>::states.push_back(first);
		statesInSetForm.push_back(trueFirstIndices);
		
		NFA<AutomataState>::initialStateIndex = 0;

		for (StateIndex i = 0; i < NFA<AutomataState>::states.size(); i++)
		{
			lg("�ַ���״̬ %d", i);
			for (auto &l : NFA<AutomataState>::alphabet)
			{
				// ����Ŀ��״̬����
				bool haveAccepted = false;
				bool haveIntermediate = false;
				std::set< StateIndex> destStateIndices;

				for (auto si : statesInSetForm[i])
				{
					if (nfa.states[si].delta.count(l) == 1)
					{
						destStateIndices.insert(nfa.states[si].delta[l].begin(), nfa.states[si].delta[l].end());
					}
				}

				lg("״̬ %d ͨ�� %s �ɵ���״̬: ", i, l.getShortDescription().c_str());

				for (auto si : destStateIndices)
				{
					lg("%d", si);
				}

				destStateIndices = nfa.getEpsilonClosureOf(destStateIndices);

				lg("״̬ %d ͨ�� %s �ɵ���״̬ (�űհ���): ", i, l.getShortDescription().c_str());

				for (auto si : destStateIndices)
				{
					lg("%d", si);
				}

				haveAccepted = nfa.haveAcceptedStateIn(destStateIndices);
				haveIntermediate = nfa.haveIntermediateStateIn(destStateIndices);

				// ����, �����������״̬��
				bool duplicate = false;
				size_t statesInSetFormSize = statesInSetForm.size();
				StateIndex duplicatedStateSetIndex = 0;
				for (StateIndex j = 0; j < statesInSetFormSize; j++)
				{
					if (destStateIndices == statesInSetForm[j])
					{
						duplicate = true;
						duplicatedStateSetIndex = j;
						lg("����״̬ %d �ظ�", j);
						break;
					}
				}

				if (!duplicate)
				{
					AutomataState curr;
					//curr.description = nfa.makeDescription(destStateIndices);
					curr.tag = haveAccepted ? AutomataStateTag::accepted : haveIntermediate ? AutomataStateTag::intermediate : AutomataStateTag::notAccepted;
					nfa.mergeByStateIndices(curr, destStateIndices);

					NFA<AutomataState>::states.push_back(curr);
					statesInSetForm.push_back(destStateIndices);

					NFA<AutomataState>::states[i].delta[l] = std::set<StateIndex>{ StateIndex(NFA<AutomataState>::states.size() - 1) };
					lg("��������״̬ %d : %s, %s", NFA<AutomataState>::states.size() - 1, curr.description.c_str(), automataStateTagText[curr.tag]);
				}
				else
				{
					NFA<AutomataState>::states[i].delta[l] = std::set<StateIndex>{ duplicatedStateSetIndex }; ;
				}
			}
		}

		// ���һ�����������״̬, ���κ�״̬���յ���ĸ����û�е���ĸ����������
		AutomataState curr;
		curr.description = "(extra empty)";
		curr.tag = AutomataStateTag::notAccepted;

		NFA<AutomataState>::states.push_back(curr);

		lg("������������״̬");
	}

	template <class AutomataState>
	void AkaneLang::DFA<AutomataState>::validate() const
	{
		auto alphabetSize = NFA<AutomataState>::alphabet.size();
		bool haveInitialState = false;
		// delta ������, ��ĸ���ڵ�ÿ����ĸ��Ӧ 1 ���ҽ� 1 ��Ŀ��״̬
		auto statesSize = NFA<AutomataState>::states.size();
		for (StateIndex i = 0; i < statesSize; i++)
		{
			if (statesSize && i == statesSize - 1)	// Ĭ������״̬, û��ת�ƺ�����
				break;

			auto &s = NFA<AutomataState>::states[i];

			for (auto &l : NFA<AutomataState>::alphabet)
			{
				if (s.delta.count(l) != 1)
				{
					throw AkaneLangException("״̬ %s �ڶ� %s ʱ��ת��", s.description.c_str(), l.getShortDescription().c_str());
				}
			}
			for (auto &dPair : s.delta)
			{
				if (dPair.second.size() != 1)
				{
					throw AkaneLangException("״̬ %s �� %s ת��״̬����Ϊ 1, Ϊ %d", s.description.c_str(), dPair.first.getShortDescription().c_str(), dPair.second.size());
				}
			}
		}
		haveInitialState = NFA<AutomataState>::initialStateIndex >= 0 && NFA<AutomataState>::initialStateIndex < NFA<AutomataState>::states.size();
		if (!haveInitialState)
		{
			throw AkaneLangException("�Ҳ�����ʼ״̬");
		}
	}

	template <class AutomataState>
	void AkaneLang::DFA<AutomataState>::goOneStep()
	{
		if (currStateIndex < 0 || currStateIndex >= this->states.size())
		{
			throw AkaneLangException("currStateIndex ����: %d", currStateIndex);
		}

		GET_ASSIGN_AND_FREE(StringifiedLetter, currLetter, (*NFA<AutomataState>::letterGeneratorP->next_freeNeeded()));

		if (this->tagifyMap.count(currLetter) == 1)
		{
			currLetter = this->tagifyMap[currLetter];
		}

		if (NFA<AutomataState>::states[currStateIndex].delta.count(currLetter) == 0)
		{
			if (this->isNotMatchedLettersMappedToElse)
			{
				currLetter = StringifiedLetter::elseLetter();
			}
			else
			{
				currStateIndex = NFA<AutomataState>::states.size() - 1;	// ��������
				return;
			}
		}
		
		currStateIndex = *(NFA<AutomataState>::states[currStateIndex].delta[currLetter].cbegin());
	}

	template <class AutomataState>
	void AkaneLang::DFA<AutomataState>::goOneStep_peek()
	{
		if (currStateIndex < 0 || currStateIndex >= this->states.size())
		{
			throw AkaneLangException("currStateIndex ����: %d", currStateIndex);
		}
		
		GET_ASSIGN_AND_FREE(StringifiedLetter, currLetter, (*NFA<AutomataState>::letterGeneratorP->peek_freeNeeded()))
			;

		if (this->tagifyMap.count(currLetter) == 1)
		{
			currLetter = this->tagifyMap[currLetter];
		}

		if (NFA<AutomataState>::states[currStateIndex].delta.count(currLetter) == 0)
		{
			if (this->isNotMatchedLettersMappedToElse)
			{
				currLetter = StringifiedLetter::elseLetter();
			}
			else
			{
				currStateIndex = NFA<AutomataState>::states.size() - 1;	// ��������
				return;
			}
		}

		currStateIndex = *(NFA<AutomataState>::states[currStateIndex].delta[currLetter].cbegin());
	}

	template <class AutomataState>
	AutomataStateTag DFA<AutomataState>::peekStateTag() const
	{
		if (currStateIndex < 0 || currStateIndex >= this->states.size())
		{
			throw AkaneLangException("currStateIndex ����: %d", currStateIndex);
		}
		GET_ASSIGN_AND_FREE(StringifiedLetter, l, (*NFA<AutomataState>::letterGeneratorP->peek_freeNeeded()))
			;

		if (this->tagifyMap.count(l) == 1)
		{
			l = this->tagifyMap.at(l);
		}

		if (NFA<AutomataState>::states[currStateIndex].delta.count(l) == 0)
		{
			if (this->isNotMatchedLettersMappedToElse)
			{
				l = StringifiedLetter::elseLetter();
			}
			else
			{
				return AutomataStateTag::notAccepted;
			}
		}

		return NFA<AutomataState>::states[*(NFA<AutomataState>::states[currStateIndex].delta.at(l).cbegin())].tag;
	}

	template <class AutomataState>
	void AkaneLang::DFA<AutomataState>::reset()
	{
		currStateIndex = NFA<AutomataState>::initialStateIndex;
	}

	template <class AutomataState>
	std::string DFA<AutomataState>::typeStr() const
	{
		return std::string("DFA");
	}
/*
	template<class AutomataState>
	inline void DFA<AutomataState>::setCurrStateIndex(size_t _currStateIndex) { currStateIndex = _currStateIndex; }

	template<class AutomataState>
	inline size_t DFA<AutomataState>::getCurrStateIndex() { return currStateIndex; }
*/
	template <class AutomataState>
	AutomataStateTag AkaneLang::DFA<AutomataState>::nowStateTag() const
	{
		return NFA<AutomataState>::states[currStateIndex].tag;
	}

	template struct NFA<BaseState>;
	template struct DFA<BaseState>;

	template struct NFA<ItemState>;
	template struct DFA<ItemState>;

	template void NFA<BaseState>::print<AkaneUtils::Logger>(AkaneUtils::Logger &out) const;
	template void NFA<ItemState>::print<AkaneUtils::Logger>(AkaneUtils::Logger &out) const;

	template void NFA<BaseState>::print<std::ostream>(std::ostream &out) const;
	template void NFA<ItemState>::print<std::ostream>(std::ostream &out) const;
}

