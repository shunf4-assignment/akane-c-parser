#define _CRT_SECURE_NO_WARNINGS
#include "akane-lang.h"
#include <memory>

namespace AkaneLang
{
	const char *automataStateTagText[] =
	{
		"拒绝",
		"接受",
		"中间"
	};

	const StateIndex invalidStateIndex = std::numeric_limits<StateIndex>::max();

	template class AkaneLang::NFA<LetterString, SimpleState>;
	template class AkaneLang::DFA<LetterString, SimpleState>;

	template <class L, template <class LL> class AutomataState>
	AkaneLang::NFA<L, AutomataState>::NFA(const std::vector<L>& _alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator * _letterGenP) : alphabet(_alphabet), tagifyMap(_tagifyMap), letterGenP(_letterGenP)
	{
		char tmpStr[50] = { 0 };
#pragma warning(push)
#pragma warning(disable:4311 4302)
		sprintf(tmpStr, "FA-%08x", (unsigned __int32)(this));
#pragma warning(pop)
		name = tmpStr;
		initialStateIndex = invalidStateIndex;
	}

	template <class L, template <class LL> class AutomataState>
	AkaneLang::NFA<L, AutomataState>::NFA() : letterGenP((static_cast<LetterGenerator *>(nullptr)))
	{
		char tmpStr[50] = { 0 };
#pragma warning(push)
#pragma warning(disable:4311 4302)
		sprintf(tmpStr, "FA-%08x", (unsigned __int32)(this));
#pragma warning(pop)
		name = tmpStr;
		initialStateIndex = invalidStateIndex;
	}

	template <class L, template <class LL> class AutomataState>
	std::string AkaneLang::NFA<L, AutomataState>::typeStr() const
	{
		return std::string("NFA");
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::NFA<L, AutomataState>::addState(AutomataState<L> state)
	{
		states.push_back(state);
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::NFA<L, AutomataState>::loadAlphabet(const std::vector<L>& _alphabet)
	{
		alphabet = _alphabet;
	}

	template <class L, template <class LL> class AutomataState>
	void NFA<L, AutomataState>::loadtagifyMap(const std::map<L, L>& _tagifyMap)
	{
		tagifyMap = _tagifyMap;
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::NFA<L, AutomataState>::loadStates(const std::vector<AutomataState<L>>& _states)
	{
		states.clear();
		states.insert(states.end(), _states.begin(), _states.end());
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::NFA<L, AutomataState>::loadDeltas(std::vector<std::map<L, std::set<StateIndex>>>& _indexedDeltas)
	{
		if (_indexedDeltas.size() != states.size())
		{
			throw AkaneLangException("_indexedDeltas 大小不符合");
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

	template <class L, template <class LL> class AutomataState>
	std::set<StateIndex> AkaneLang::NFA<L, AutomataState>::epsilonClosure(const std::set<StateIndex>& sis)
	{
		std::set<StateIndex> result;
		for (auto si : sis)
		{
			result.insert(si);
		}

		size_t prevSize = result.size();
		while (true)
		{
			// 遍历时插入有坑?
			for (auto si : result)
			{
				if (states[si].delta.count(L::epsilon()) == 1)
				{
					auto &epsilonSet = states[si].delta[L::epsilon()];
					result.insert(epsilonSet.begin(), epsilonSet.end());
				}
			}
			if (result.size() == prevSize)
				break;
			prevSize = result.size();
		}
		return result;
	}

	template void AkaneLang::NFA<LetterString, SimpleState>::print<AkaneUtils::Logger>(AkaneUtils::Logger &out);

	template <class L, template <class LL> class AutomataState>
	template <class Out>
	void AkaneLang::NFA<L, AutomataState>::print(Out &out)
	{
		using namespace std;
		out << "==== FA 打印 ====" << endl;
		out << "名称: " << name << " ("<< this->typeStr() << ")" << endl;
		out << "字母表: " << endl;
		for (auto &l : alphabet)
		{
			out << l.getDescription();
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
					
					out << p.first.getDescription() << " ";
				}
			}
			out << endl;
		}
		out << endl;

		out << "初始状态序号: " << initialStateIndex << endl;

		for (unsigned i = 0; i < states.size(); i++)
		{
			out << "状态" << i << " : <" << states[i].description << "> " << automataStateTagText[states[i].tag] << ". ";
			out << "转移函数 : " << endl;

			bool empty = true;
			for (auto &dPair : states[i].delta)
			{
				if (empty)
					empty = false;
				out << dPair.first.getDescription() << " -> ";
				if (dPair.second.size() > 0) {
					for (auto &destStateIndex : dPair.second)
					{
						out << destStateIndex << " <" << states[destStateIndex].description << ">;";
					}
					out << endl;
				}
				else out << "(无)  ";
			}
			if (empty)
			{
				out << "(空)";
			}

			out << endl;
		}
		out << "是否通过检测?" << endl;
		try
		{
			validate();
			out << "是" << endl;
		}
		catch (AkaneLangException)
		{
			out << "否" << endl;
		}
		out << endl;
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::NFA<L, AutomataState>::validate()
	{
		;
	}

	template <class L, template <class LL> class AutomataState>
	inline std::string AkaneLang::NFA<L, AutomataState>::makeDescription(std::set<StateIndex>& _stateSet)
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

	template <class L, template <class LL> class AutomataState>
	inline bool AkaneLang::NFA<L, AutomataState>::haveAcceptedState(std::set<StateIndex>& _stateSet)
	{
		bool haveAcceptedState = false;
		for (auto si : _stateSet)
		{
			if (states[si].tag == AutomataStateTag::accepted)
			{
				haveAcceptedState = true;
				break;
			}
		}

		return haveAcceptedState;
	}

	template <class L, template <class LL> class AutomataState>
	inline bool AkaneLang::NFA<L, AutomataState>::haveIntermediateState(std::set<StateIndex>& _stateSet)
	{
		bool haveIntermediateState = false;
		for (auto si : _stateSet)
		{
			if (states[si].tag == AutomataStateTag::intermediate)
			{
				haveIntermediateState = true;
				break;
			}
		}

		return haveIntermediateState;
	}

	template <class L, template <class LL> class AutomataState>
	AkaneLang::DFA<L, AutomataState>::DFA(const std::vector<L>& _alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator * _letterGenP) : AkaneLang::NFA<L, AutomataState>::NFA(_alphabet, _tagifyMap, _letterGenP)
	{
		currStateIndex = invalidStateIndex;
	}


	template <class L, template <class LL> class AutomataState>
	AkaneLang::DFA<L, AutomataState>::DFA(NFA<L, AutomataState>& nfa)
	{
		this->DFA<L, AutomataState>::DFA(nfa.alphabet, nfa.tagifyMap, nfa.letterGenP);

		lg("==== 开始将 NFA %s 转为 DFA %s ====", nfa.name.c_str(), NFA<L, AutomataState>::name.c_str());
		std::vector< std::set< StateIndex>> statesInSetForm;
		this->states.clear();
		// 创建第一个状态
		AutomataState< L> first;
		std::set <StateIndex> tmpFirst = std::set< StateIndex>{ nfa.initialStateIndex };
		lg("第一个状态: %d", nfa.initialStateIndex);
		std::set <StateIndex> trueFirst = nfa.epsilonClosure(tmpFirst);
		lg("第一个状态闭包后: ");
		for (auto si : trueFirst)
		{
			lg("%d", si);
		}

		first.description = nfa.makeDescription(trueFirst);
		first.tag = nfa.haveAcceptedState(trueFirst) ? AutomataStateTag::accepted : nfa.haveIntermediateState(trueFirst) ? AutomataStateTag::intermediate : AutomataStateTag::notAccepted;

		NFA<L, AutomataState>::states.push_back(first);
		statesInSetForm.push_back(trueFirst);
		
		NFA<L, AutomataState>::initialStateIndex = 0;

		for (StateIndex i = 0; i < NFA<L, AutomataState>::states.size(); i++)
		{
			lg("现分析状态 %d", i);
			for (auto &l : NFA<L, AutomataState>::alphabet)
			{
				// 生成目标状态集合
				bool haveAccepted = false;
				bool haveIntermediate = false;
				std::set< StateIndex> destStates;

				for (auto si : statesInSetForm[i])
				{
					if (nfa.states[si].delta.count(l) == 1)
					{
						destStates.insert(nfa.states[si].delta[l].begin(), nfa.states[si].delta[l].end());
					}
				}

				lg("状态 %d 通过 %s 可到达状态: ", i, l.getDescription().c_str());

				for (auto si : destStates)
				{
					lg("%d", si);
				}

				destStates = nfa.epsilonClosure(destStates);

				lg("状态 %d 通过 %s 可到达状态 (ε闭包后): ", i, l.getDescription().c_str());

				for (auto si : destStates)
				{
					lg("%d", si);
				}

				haveAccepted = nfa.haveAcceptedState(destStates);
				haveIntermediate = nfa.haveIntermediateState(destStates);

				// 查重, 不重则插入新状态中
				bool duplicate = false;
				size_t statesInSetFormSize = statesInSetForm.size();
				StateIndex duplicatedStateSetIndex = 0;
				for (StateIndex j = 0; j < statesInSetFormSize; j++)
				{
					if (destStates == statesInSetForm[j])
					{
						duplicate = true;
						duplicatedStateSetIndex = j;
						lg("与新状态 %d 重复", j);
						break;
					}
				}

				if (!duplicate)
				{
					AutomataState< L> curr;
					curr.description = nfa.makeDescription(destStates);
					curr.tag = haveAccepted ? AutomataStateTag::accepted : haveIntermediate ? AutomataStateTag::intermediate : AutomataStateTag::notAccepted;

					NFA<L, AutomataState>::states.push_back(curr);
					statesInSetForm.push_back(destStates);

					NFA<L, AutomataState>::states[i].delta[l] = std::set<StateIndex>{ StateIndex(NFA<L, AutomataState>::states.size() - 1) };
					lg("创建了新状态 %d : %s, %s", NFA<L, AutomataState>::states.size() - 1, curr.description.c_str(), automataStateTagText[curr.tag]);
				}
				else
				{
					NFA<L, AutomataState>::states[i].delta[l] = std::set<StateIndex>{ duplicatedStateSetIndex }; ;
				}
			}
		}

		// 添加一个额外的陷阱状态, 当任何状态接收到字母表中没有的字母就跳到这里
		AutomataState< L> curr;
		curr.description = "(extra empty)";
		curr.tag = AutomataStateTag::notAccepted;

		NFA<L, AutomataState>::states.push_back(curr);

		lg("创建了新陷阱状态");
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::DFA<L, AutomataState>::validate()
	{
		auto alphabetSize = NFA<L, AutomataState>::alphabet.size();
		bool haveInitialState = false;
		// delta 函数内, 字母表内的每个字母对应 1 个且仅 1 个目标状态
		auto statesSize = NFA<L, AutomataState>::states.size();
		for (StateIndex i = 0; i < statesSize; i++)
		{
			if (i == statesSize - 1)	// 默认陷阱状态, 没有转移函数项
				break;

			auto &s = NFA<L, AutomataState>::states[i];

			for (auto &l : NFA<L, AutomataState>::alphabet)
			{
				if (s.delta.count(l) != 1)
				{
					throw AkaneLangException("状态 %s 在读 %s 时不转移", s.description.c_str(), l.getValue().c_str());
				}
			}
			for (auto &dPair : s.delta)
			{
				if (dPair.second.size() != 1)
				{
					throw AkaneLangException("状态 %s 的 %s 转移状态数不为 1, 为 %d", s.description.c_str(), dPair.first.getValue().c_str(), dPair.second.size());
				}
			}
		}
		haveInitialState = NFA<L, AutomataState>::initialStateIndex >= 0 && NFA<L, AutomataState>::initialStateIndex < NFA<L, AutomataState>::states.size();
		if (!haveInitialState)
		{
			throw AkaneLangException("找不到初始状态");
		}
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::DFA<L, AutomataState>::oneStep()
	{
		if (currStateIndex < 0 || currStateIndex >= this->states.size())
		{
			throw AkaneLangException("currStateIndex 不好: %d", currStateIndex);
		}

		GET_ASSIGN_AND_FREE(const L, lC, (NFA<L, AutomataState>::letterGenP->next_freeNeeded()))
			;
		L l = lC;
		if (this->tagifyMap.count(l) == 1)
		{
			l = this->tagifyMap[l];
		}

		if (NFA<L, AutomataState>::states[currStateIndex].delta.count(l) == 0)
			currStateIndex = NFA<L, AutomataState>::states.size() - 1;	// 掉入陷阱
		else
			currStateIndex = *(NFA<L, AutomataState>::states[currStateIndex].delta[l].cbegin());
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::DFA<L, AutomataState>::oneStep_peek()
	{
		if (currStateIndex < 0 || currStateIndex >= this->states.size())
		{
			throw AkaneLangException("currStateIndex 不好: %d", currStateIndex);
		}
		
		GET_ASSIGN_AND_FREE(const L, lC, (NFA<L, AutomataState>::letterGenP->peek_freeNeeded()))
			;
		L l = lC;
		if (this->tagifyMap.count(l) == 1)
		{
			l = this->tagifyMap[l];
		}

		if (NFA<L, AutomataState>::states[currStateIndex].delta.count(l) == 0)
			currStateIndex = NFA<L, AutomataState>::states.size() - 1;	// 掉入陷阱
		else
			currStateIndex = *(NFA<L, AutomataState>::states[currStateIndex].delta[l].cbegin());
	}

	template <class L, template <class LL> class AutomataState>
	AutomataStateTag DFA<L, AutomataState>::peekStateTag()
	{
		if (currStateIndex < 0 || currStateIndex >= this->states.size())
		{
			throw AkaneLangException("currStateIndex 不好: %d", currStateIndex);
		}
		GET_ASSIGN_AND_FREE(const L, lC, (NFA<L, AutomataState>::letterGenP->peek_freeNeeded()))
			;
		L l = lC;
		if (this->tagifyMap.count(l) == 1)
		{
			l = this->tagifyMap[l];
		}

		if (NFA<L, AutomataState>::states[currStateIndex].delta.count(l) == 0)
			return AutomataStateTag::notAccepted;	// 掉入陷阱
		else
			return NFA<L, AutomataState>::states[*(NFA<L, AutomataState>::states[currStateIndex].delta[l].cbegin())].tag;
	}

	template <class L, template <class LL> class AutomataState>
	void AkaneLang::DFA<L, AutomataState>::reset()
	{
		currStateIndex = NFA<L, AutomataState>::initialStateIndex;
	}

	template <class L, template <class LL> class AutomataState>
	std::string DFA<L, AutomataState>::typeStr() const
	{
		return std::string("DFA");
	}

	template <class L, template <class LL> class AutomataState>
	AutomataStateTag AkaneLang::DFA<L, AutomataState>::nowStateTag()
	{
		return NFA<L, AutomataState>::states[currStateIndex].tag;
	}
}