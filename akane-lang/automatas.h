#pragma once

namespace AkaneLang
{


	template <class L, template <class LL> class AutomataState>
	class NFA
	{
	public:
		static_assert(std::is_base_of_v<Letter, L>, "必须继承自 Letter");
		std::vector< L> alphabet;
		std::vector< AutomataState<L>> states;
		LetterGenerator *letterGenP;
		std::map<L, L> tagifyMap;
		bool isNotMatchedLettersMappedToElse;
		std::string name;

		NFA(const std::vector< L> &_alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator *_letterGenP);
		NFA();
		virtual std::string typeStr() const;
		void addState(AutomataState< L> state);
		void loadAlphabet(const std::vector< L>& _alphabet);
		void loadtagifyMap(const std::map<L, L> &_tagifyMap);
		void loadStates(const std::vector< AutomataState< L>> &_states);
		void loadDeltas(std::vector< std::map< L, std::set<StateIndex>>> &_indexedDeltas);
		std::string makeDescription(std::set< StateIndex> &_stateSet);
		bool haveAcceptedState(std::set< StateIndex> &_stateSet);
		bool haveIntermediateState(std::set<StateIndex>& _stateSet);
		void setInitialStateIndex(size_t _initialStateIndex) { initialStateIndex = _initialStateIndex; }
		size_t getInitialStateIndex() { return initialStateIndex; }
		std::set<StateIndex> epsilonClosure(const std::set<StateIndex> &ss);
		template <class Out> void print(Out &out);
		virtual void validate();
		size_t initialStateIndex;

		void merge(AutomataState<L> &dest, std::set<StateIndex> &srcIndices)
		{
			dest.startMerge();
			for (auto si : srcIndices)
			{
				dest.mergeThisState(states[si]);
			}
			dest.endMerge();
		}
	};

	// DFA 的 states 最后一个 state 是自动创建的陷阱 state, 用来在接收到无法识别的字母时跳转
	template <class L, template <class LL> class AutomataState>
	class DFA : public NFA<L, AutomataState>
	{
	public:
		DFA(const std::vector< L> &_alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator *_letterGenP);
		DFA(NFA<L, AutomataState> &nfa);
		virtual void validate();
		void oneStep();
		void oneStep_peek();
		AutomataStateTag peekStateTag();
		void reset();
		virtual std::string typeStr() const;
		void setCurrStateIndex(size_t _currStateIndex) { currStateIndex = _currStateIndex; }
		size_t getCurrStateIndex() { return currStateIndex; }
		AutomataStateTag nowStateTag();
	private:
		size_t currStateIndex;
	};
}