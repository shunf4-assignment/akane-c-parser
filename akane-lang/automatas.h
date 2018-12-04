#pragma once

namespace AkaneLang
{
	template <class AutomataState>
	struct NFA
	{
		static_assert(std::is_base_of_v<BaseState, AutomataState>, "����̳��� BaseState");

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
	// DFA �� states ���һ�� state ���Զ����������� state, �����ڽ��յ��޷�ʶ�����ĸʱ��ת
	template <class AutomataState>
	struct DFA : public NFA<AutomataState>
	{
		size_t currStateIndex;
		
		DFA();
		//DFA(const std::vector<StringifiedLetter> &_alphabet, const std::map<StringifiedLetter, StringifiedLetter> &_tagifyMap, LetterGenerator *_letterGeneratorP);
		// ���� NFA ���챾 DFA.
		DFA(NFA<AutomataState> &nfa);
		// ��֤�� DFA ��ȷ��.
		virtual void validate() const override;
		// ����һ�� DFA.
		void goOneStep();
		void goOneStep_peek();
		AutomataStateTag nowStateTag() const;
		AutomataStateTag peekStateTag() const;
		void reset();
		std::string typeStr() const override;
	};
}