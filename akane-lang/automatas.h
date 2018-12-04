#pragma once

namespace AkaneLang
{
	template <class AutomataState>
	struct NFA
	{
		static_assert(std::is_base_of_v<BaseState, AutomataState>, "必须继承自 BaseState");

		std::vector<StringifiedLetter> alphabet;
		std::vector<AutomataState> states;
		LetterGenerator *letterGeneratorP;
		std::map<StringifiedLetter, StringifiedLetter> tagifyMap;
		bool isNotMatchedLettersMappedToElse;
		std::string name;
		StateIndex initialStateIndex;

		NFA();
		NFA(const std::vector<StringifiedLetter> &_alphabet, const std::vector<AutomataState> &_states, const std::map<StringifiedLetter, StringifiedLetter> &_tagifyMap, LetterGenerator *_letterGeneratorP, const std::string &_name, StateIndex _initialStateIndex);
		virtual std::string typeStr() const;
		/*
		void addState(AutomataState _state);
		void loadAlphabet(const std::vector<StringifiedLetter>& _alphabet);
		void loadtagifyMap(const std::map<StringifiedLetter, StringifiedLetter> &_tagifyMap);
		void loadStates(const std::vector<AutomataState> &_states);
		void loadDeltas(std::vector< std::map<StringifiedLetter, std::set<StateIndex>>> &_indexedDeltas);
		*/
		bool haveAcceptedStateIn(const std::set<StateIndex> &_stateSet) const;
		bool haveIntermediateStateIn(const std::set<StateIndex>& _stateSet) const;
		std::set<StateIndex> getEpsilonClosureOf(const std::set<StateIndex> &_stateSet) const;
		virtual void validate() const;
		void mergeByStateIndices(AutomataState &dest, const std::set<StateIndex> &srcIndices) const;
		template <class Out> void print(Out &out) const;
	};

	// DFA.
	// DFA 的 states 最后一个 state 是自动创建的陷阱 state, 用来在接收到无法识别的字母时跳转
	template <class AutomataState>
	struct DFA : public NFA<AutomataState>
	{
		size_t currStateIndex;
		
		DFA();
		//DFA(const std::vector<StringifiedLetter> &_alphabet, const std::map<StringifiedLetter, StringifiedLetter> &_tagifyMap, LetterGenerator *_letterGeneratorP);
		// 根据 NFA 构造本 DFA.
		DFA(NFA<AutomataState> &nfa);
		// 验证本 DFA 正确性.
		virtual void validate() const override;
		// 运行一步 DFA.
		void goOneStep();
		void goOneStep_peek();
		AutomataStateTag nowStateTag() const;
		AutomataStateTag peekStateTag() const;
		void reset();
		std::string typeStr() const override;
	};
}