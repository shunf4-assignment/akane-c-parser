#pragma once
#include "..\akane-lex\akane-lex.h"
#include "parse-tree.h"

DEFINE_AKANE_EXCEPTION(Parse)

namespace AkaneParse
{
#include "internal-usings.h"
	struct LRAnalysisAction
	{
	private:
		std::string type;
		std::string text;
	public:
		LRAnalysisAction();
		LRAnalysisAction(const std::string &_type, const std::string &_text);

		static const LRAnalysisAction &shift();
		static const LRAnalysisAction &reduce();

		bool operator==(const LRAnalysisAction &right) const;

		const std::string &getDescription() const;

	};

	typedef Index StateOrProductionIndex;

	class LRAnalysisTable
	{
	public:
		size_t statesSize;
		size_t terminalsSize;
		size_t nonTerminalsSize;
		const Grammar *grammarP;
		std::map<std::pair<StateIndex, StringifiedGrammarSymbol>, std::pair<LRAnalysisAction, StateOrProductionIndex>> ACTION;
		std::map<std::pair<StateIndex, StringifiedGrammarSymbol>, StateIndex> GOTO;
		//std::map<StringifiedGrammarSymbol, SymbolIndex> terminalToIndex;

		LRAnalysisTable();

		LRAnalysisTable(const Grammar &_grammar, const std::vector<LRItem> &lrItems, const DFA<ItemState> &_dfa);
		/*
		void mapTerminalToIndex()
		{
			terminalToIndex.clear(); 
			for (Index i = 0; i < grammarP->symbols.size(); i++)
			{
				auto &sym = dynamic_cast<AkaneLang::NamedGrammarSymbol &>(*(grammarP->symbols[i]));
				terminalToIndex[sym] = i;
			}
		}*/

		void insertACTION(const std::pair<StateIndex, StringifiedGrammarSymbol> &left, const std::pair<LRAnalysisAction, StateOrProductionIndex> &right);

		void insertGOTO(const std::pair<StateIndex, StringifiedGrammarSymbol> &pair, StateIndex nextStateIndex);

		template <class Out> void print(Out &out) const;
	};

	class LR1Parser
	{
	public:
		TokenizedLetterGenerator &inputTokenizedLetterGenerator;
		ParseTree outputParseTree;

		AkaneLang::Grammar grammar;
		std::vector<AkaneLang::LRItem> lrItems;
		DFA<ItemState> dfa;
		LRAnalysisTable table;

		LR1Parser(std::istream &grammarF, TokenizedLetterGenerator &_inputTokenizedLetterGenerator);

		void insertLRItemAndState(std::vector<ItemState> &_states, Index sourceStateIndex, AutomataStateTag nextStateTag, const StringifiedGrammarSymbol &currSymbol, LRItem &newItem);

		std::vector<ItemState> makeItemStates();
		void makeDFA();
		template <class Out> void printAnalysisStep(Out &out, Index stepI, const std::vector<std::tuple<StateIndex, StringifiedGrammarSymbol, Index>> &stack);
		void analyze();
	};
	
}