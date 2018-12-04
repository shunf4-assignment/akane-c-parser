#include "stdafx.h"
#include "..\akane-lang\akane-lang.h"
#include "..\akane-lang\utils.h"
#include "..\akane-lex\akane-lex.h"
#include "akane-parse.h"
/*namespace AkaneParse
{
	class Parser
	{
		AkaneLex::LetterTokenGenerator &outputTokenizedLetterGenerator;
		Parser(AkaneLex::LetterTokenGenerator &_outputTokenizedLetterGenerator) : outputTokenizedLetterGenerator(_outputTokenizedLetterGenerator)
		{

		}

		void analyze()
		{
			Grammar g = readGrammar();
			
		}
	};
}*/
using namespace AkaneParse;

static const size_t endState = (std::numeric_limits<StateIndex>::max)() - 1;
static const size_t noNodeIndex = (std::numeric_limits<StateIndex>::max)() - 1;

inline LRAnalysisTable::LRAnalysisTable() : statesSize(0), terminalsSize(0), nonTerminalsSize(0), grammarP(nullptr)
{
}

inline AkaneParse::LRAnalysisTable::LRAnalysisTable(const Grammar & _grammar, const std::vector<LRItem>& lrItems, const DFA<ItemState>& _dfa) : terminalsSize(0), nonTerminalsSize(0)
{
	// 初始化
	grammarP = &_grammar;
	statesSize = _dfa.states.size() - 1;	// 剪掉 NFA 转 DFA 额外加的一个陷阱状态

	for (Index symI = 0; symI < _grammar.symbols.size(); symI++)
	{
		auto &currSym = dynamic_cast<const StringifiedGrammarSymbol &>(*_grammar.symbols[symI]);

		if (currSym.isTerminal())
		{
			terminalsSize++;
		}
		else
		{
			nonTerminalsSize++;
		}
	}

	// 对每个状态
	for (StateIndex stateI = 0; stateI < _dfa.states.size(); stateI++)
	{
		auto &currState = _dfa.states[stateI];

		// 对每个符号
		for (Index symI = 0; symI < _grammar.symbols.size(); symI++)
		{
			const StringifiedGrammarSymbol &currSym = dynamic_cast<const StringifiedGrammarSymbol &>(*_grammar.symbols[symI]);

			if (currState.delta.count(currSym) == 0)
				continue;

			StateIndex nextStateIndex = currState.delta.at(currSym).begin().operator*();

			auto &nextState = _dfa.states[nextStateIndex];

			if (currSym.isTerminal())
			{
				if (nextState.itemIndices.size() == 0)
					continue;

				insertACTION({ stateI, currSym }, { LRAnalysisAction::shift(), nextStateIndex });
			}
			else
			{
				if (nextState.itemIndices.size() == 0)
					continue;

				insertGOTO({ stateI, currSym }, nextStateIndex);
			}
		}

		for (auto lrII : currState.itemIndices)
		{
			if (lrItems[lrII].isReducing())
			{
				insertACTION({ stateI, dynamic_cast<const StringifiedGrammarSymbol &>(*_grammar.symbols[lrItems[lrII].lookAheadIndex]) }, { LRAnalysisAction::reduce(), lrItems[lrII].productionP->indexInGrammar });
			}
		}
	}
}

inline void LRAnalysisTable::insertACTION(const std::pair<StateIndex, StringifiedGrammarSymbol>& left, const std::pair<LRAnalysisAction, StateOrProductionIndex>& right)
{
	auto &stateI = left.first;
	auto &symbol = left.second;

	const std::string &currSymStr = symbol.getShortDescription();

	if (ACTION.count({ stateI, symbol }))
	{
		auto &a = ACTION.at({stateI, symbol});
		const char *cStr = currSymStr.c_str();
		std::stringstream tmpStream1;
		std::stringstream tmpStream2;

		if (a.first == LRAnalysisAction::shift())
			tmpStream1 << a.second;
		else
			tmpStream1 << grammarP->productions[a.second].getDescription();

		if (right.first == LRAnalysisAction::shift())
			tmpStream2 << right.second;
		else
			tmpStream2 << grammarP->productions[right.second].getDescription();

		throw AkaneParseException("ACTION[%u, %s] = %s:%s 与 ACTION[%u, %s] = %s:%s 冲突, 该文法不是 LR(1) 的.", stateI, cStr, a.first.getDescription().c_str(), tmpStream1.str().c_str(), stateI, cStr, right.first.getDescription().c_str(), tmpStream2.str().c_str());
	}
	else
	{
		ACTION[left] = right;
	}
}

inline void LRAnalysisTable::insertGOTO(const std::pair<StateIndex, StringifiedGrammarSymbol>& pair, StateIndex nextStateIndex)
{
	auto &stateI = pair.first;
	auto &symbol = pair.second;
	const std::string &currSymStr = symbol.getShortDescription();
	if (GOTO.count({ stateI, symbol }))
	{
		auto g = GOTO[{stateI, symbol}];
		const char *cStr = currSymStr.c_str();

		throw AkaneParseException("GOTO[%u, %s] = %u 与 GOTO[%u, %s] = %u 冲突.", stateI, cStr, g, stateI, cStr, nextStateIndex);
	}
	else
	{
		GOTO[pair] = nextStateIndex;
	}
}

AkaneParse::LRAnalysisAction::LRAnalysisAction()
{
}

AkaneParse::LRAnalysisAction::LRAnalysisAction(const std::string & _type, const std::string & _text) : type(_type), text(_text)
{
}

inline const LRAnalysisAction & LRAnalysisAction::shift()
{
	static LRAnalysisAction a = LRAnalysisAction("shift", "移进");
	return a;
}

inline const LRAnalysisAction & LRAnalysisAction::reduce()
{
	static LRAnalysisAction a = LRAnalysisAction("reduce", "归约");
	return a;
}

bool LRAnalysisAction::operator==(const LRAnalysisAction &right) const
{
	return type == right.type;
}

inline const std::string & LRAnalysisAction::getDescription() const
{
	return text;
}

template<class Out>
inline void LRAnalysisTable::print(Out & out) const
{
	out << std::resetiosflags(std::ios::left);
	out << std::setiosflags(std::ios::right);

	out << "==== ACTION/GOTO 表 ====" << std::endl;
	out << std::endl;
	out << "状态数 : " << statesSize << std::endl;
	out << std::endl;
	out << "|      " << "|= ACTION = " << std::setw(12 * terminalsSize - 12) << "" << "|=  GOTO  = " << std::setw(4 * nonTerminalsSize - 12) << "" << std::endl;
	out << "|      |";

	out << std::resetiosflags(std::ios::right);
	out << std::setiosflags(std::ios::left);

	for (auto &sym : grammarP->symbols)
	{
		StringifiedGrammarSymbol &symN = dynamic_cast<StringifiedGrammarSymbol &>(*sym);
		if (symN.isTerminal())
		{
			out << std::setw(12) << symN.getShortDescription();
		}
	}
	for (auto &sym : grammarP->symbols)
	{
		StringifiedGrammarSymbol &symN = dynamic_cast<StringifiedGrammarSymbol &>(*sym);
		if (!symN.isTerminal())
		{
			out << std::setw(12) << symN.getShortDescription();
		}
	}
	out << std::endl;

	for (StateIndex i = 0; i < statesSize; i++)
	{
		out << std::resetiosflags(std::ios::left);
		out << std::setiosflags(std::ios::right);
		out << "|" << std::setw(6) << i << "|";
		out << std::resetiosflags(std::ios::right);
		out << std::setiosflags(std::ios::left);
		for (Index j = 0; j < grammarP->symbols.size(); j++)
		{
			const StringifiedGrammarSymbol &symN = dynamic_cast<const StringifiedGrammarSymbol &>(*grammarP->symbols[j]);
			if (symN.isTerminal())
			{
				auto findIt = ACTION.find({ i, symN });
				if (findIt != ACTION.end())
				{
					auto &p = findIt->second;
					std::ostringstream os;
					os << p.first.getDescription() << ":";
					os << p.second;
					out << std::setw(12) << os.str();
				}
				else
				{
					out << std::setw(12) << "(ERROR)";
				}
			}
		}
		for (Index j = 0; j < grammarP->symbols.size(); j++)
		{
			const StringifiedGrammarSymbol &symN = dynamic_cast<const StringifiedGrammarSymbol &>(*grammarP->symbols[j]);
			if (!symN.isTerminal())
			{
				auto findIt = GOTO.find({ i, symN });
				if (findIt != GOTO.end())
				{
					auto nsi = findIt->second;

					out << std::setw(12) << nsi;
				}
				else
				{
					out << std::setw(12) << "(ERROR)";
				}
			}
		}
		out << std::endl;
	}
}

LR1Parser::LR1Parser(std::istream &grammarF, TokenizedLetterGenerator & _inputTokenizedLetterGenerator) : inputTokenizedLetterGenerator(_inputTokenizedLetterGenerator), grammar(grammarF), dfa()
{
	makeDFA();
	table.LRAnalysisTable::LRAnalysisTable(grammar, lrItems, dfa);
}

void LR1Parser::insertLRItemAndState(std::vector<ItemState>& _states, Index sourceStateIndex, AutomataStateTag nextStateTag, const StringifiedGrammarSymbol & currSymbol, LRItem & newItem)
{
	lrItems.push_back(newItem);
	ItemState newState = ItemState(&lrItems, { lrItems.size() - 1 }, nextStateTag);

	auto findSameItemIt = std::find(lrItems.begin(), lrItems.end() - 1, newItem);
	auto findSameStateIt = std::find(_states.begin(), _states.end(), newState);

	if (findSameItemIt == lrItems.end() - 1)
	{
		_states.push_back(newState);

		_states[sourceStateIndex].delta[currSymbol].insert(_states.size() - 1);
	}
	else if (findSameStateIt == _states.end())
	{
		Index sameItemIndex = std::distance(lrItems.begin(), findSameItemIt);
		newState.itemIndices.clear();
		newState.itemIndices.insert(sameItemIndex);
		_states.push_back(newState);

		_states[sourceStateIndex].delta[currSymbol].insert(_states.size() - 1);

		lrItems.pop_back();
	}
	else
	{
		_states[sourceStateIndex].delta[currSymbol].insert(std::distance(_states.begin(), findSameStateIt));
		lrItems.pop_back();
	}
}

inline std::vector<ItemState> LR1Parser::makeItemStates()
{
	std::vector<ItemState> states;

	// 一个初态
	states.push_back(ItemState(&lrItems, {}, AkaneLang::AutomataStateTag::notAccepted));
	states[0].description = "(初始空)";

	// 找出开始符号对应的产生式
	for (auto &prod : grammar.productions)
	{
		if (prod.leftIndices.size() == 1 && prod.leftIndices[0] == grammar.startSymbolIndex)
		{
			LRItem item = LRItem(prod, 0, grammar.eofSymbolIndex);
			lrItems.push_back(item);
			states.push_back(ItemState(&lrItems, { lrItems.size() - 1 }, AutomataStateTag::notAccepted));
			states[0].delta[StringifiedLetter::epsilon()].insert(states.size() - 1);
		}
	}

	Index i = 1;
	// 增加项目状态, 直到 states 不再增长
	for (; i < states.size(); i++)
	{
		auto currItem = lrItems[*states[i].itemIndices.begin()];
		auto &currProd = *currItem.productionP;

		if (currItem.dotPosition == currProd.rightIndices.size())
		{
			// X -> ABCD.
			continue;
		}

		auto &currSymbol = dynamic_cast<const StringifiedGrammarSymbol &>(*currProd.grammarP->symbols[currProd.rightIndices[currItem.dotPosition]]);

		// X -> AB.CD

		// 加入 Item X -> ABC.D (移进项) 并设置 delta

		LRItem shiftItem = LRItem(currItem);
		shiftItem.dotPosition++;

		AkaneLang::AutomataStateTag nextStateTag = AkaneLang::AutomataStateTag::notAccepted;
		if (shiftItem.dotPosition == shiftItem.productionP->rightIndices.size() && shiftItem.lookAheadIndex == grammar.eofSymbolIndex)
		{
			nextStateTag = AutomataStateTag::accepted;
		}

		insertLRItemAndState(states, i, nextStateTag, currSymbol, shiftItem);

		if (currSymbol.isTerminal())
		{
			// X -> AB.cD  =>  X -> ABc.D 刚刚已经做完
			continue;
		}
		else
		{
			// X -> ab.CdeF (g) => C -> ....

			std::vector<SymbolIndex> remainingSymbols;

			for (size_t pos = currItem.dotPosition + 1; pos < currProd.rightIndices.size(); pos++)
			{
				remainingSymbols.push_back(currProd.rightIndices[pos]);
			}

			remainingSymbols.push_back(currItem.lookAheadIndex);

			std::set<SymbolIndex> FIRSTofRemainingSymbols = grammar.getFIRST(remainingSymbols);

			for (auto &prod : grammar.productions)
			{
				if (prod.leftIndices.size() == 1 && prod.leftIndices[0] == currProd.rightIndices[currItem.dotPosition])
				{
					// C -> .xyz (one of FIRST(deFg))

					// 对于 FIRST(deFg) 的所有项, 都要检查并插入一遍
					for (auto oneOfFIRST : FIRSTofRemainingSymbols)
					{
						LRItem newItem = LRItem(prod, 0, oneOfFIRST);

						insertLRItemAndState(states, i, AutomataStateTag::notAccepted, StringifiedGrammarSymbol::epsilon(), newItem);
					}
				}
			}
		}

	}

	return states;
}

inline void LR1Parser::makeDFA()
{
	std::vector<StringifiedLetter> alphabet;
	for (auto symP : grammar.symbols)
	{
		if (*symP == AkaneLang::StringifiedTerminal::eof())
			continue;

		alphabet.push_back(dynamic_cast<const StringifiedLetter &>(*symP));
	}

	std::vector<ItemState> states = makeItemStates();

	// 造 NFA

	NFA<ItemState> nfa(alphabet, states, {}, &this->inputTokenizedLetterGenerator, grammar.name + " - LR(1) NFA", 0);
	nfa.isNotMatchedLettersMappedToElse = false;

	nfa.print(logger);

	// 造 DFA
	dfa.DFA<ItemState>::DFA(nfa);

	dfa.validate();
	dfa.print(logger);
}

void AkaneParse::LR1Parser::analyze()
{
	bool backup = logger.printLogToStdout;

	grammar.print(logger);
	table.print(logger);

	logger << "==== LR(1) 分析开始 ====" << std::endl;

	std::vector<std::tuple<StateIndex, StringifiedGrammarSymbol, Index>> stack;
	size_t stepI = 0;
	stack.push_back({ dfa.initialStateIndex, StringifiedGrammarSymbol::epsilon(), noNodeIndex });
	printAnalysisStep(logger, stepI, stack);

	while (true)
	{
		const TokenizedLetter &tl_tmp = *inputTokenizedLetterGenerator.peek_freeNeeded();
		const TokenizedLetter tl = tl_tmp;
		delete &tl_tmp;

		const StringifiedGrammarSymbol g = tl;
		std::pair<StateIndex, StringifiedGrammarSymbol> p(std::get<0>(stack.back()), g);
		auto findIt = table.ACTION.find(p);
		if (findIt == table.ACTION.end())
		{
			throw AkaneParseException("ACTION[%u, %s] 不存在. 输入的符号串不符合语法.", std::get<0>(stack.back()), tl.getShortDescription().c_str());
		}
		auto &action = findIt->second;

		if (action.first == LRAnalysisAction::shift())
		{
			Index nodeIndex = outputParseTree.addNode(tl);
			stack.push_back({ action.second, tl, nodeIndex });
			inputTokenizedLetterGenerator.ignoreOne();
		}
		else
		{
			SymbolIndex reducedNTIndex = grammar.productions[action.second].leftIndices[0];
			Index nodeIndex = outputParseTree.addNode(*grammar.symbols[reducedNTIndex]);
			for (Index k = 0; k < grammar.productions[action.second].rightIndices.size(); k++)
			{
				outputParseTree.nodes[nodeIndex].children.insert(outputParseTree.nodes[nodeIndex].children.begin(), std::get<2>(stack.back()));
				stack.pop_back();
			}

			StateIndex backedupPrevState = std::get<0>(stack.back());

			auto &reducedNT = dynamic_cast<const StringifiedGrammarSymbol &>(*grammar.symbols[reducedNTIndex]);

			auto findIt = table.GOTO.find({ std::get<0>(stack.back()), reducedNT });

			if (findIt == table.GOTO.end())
			{
				if (tl == TokenizedLetter::eof() && stack.size() == 1 && reducedNTIndex == grammar.startSymbolIndex)
				{
					outputParseTree.rootIndex = nodeIndex;
					stack.push_back({ endState, reducedNT, nodeIndex });
					stepI++;
					printAnalysisStep(logger, stepI, stack);

					logger << "归约到了起始非终结符, GOTO[" << backedupPrevState << ", " << reducedNT.getShortDescription() << "] 查询失败而正常退出." << std::endl;
					break;
				}
				else
				{
					throw AkaneParseException("GOTO[%u, %s] 不存在, 输入的符号串不符合语法.", std::get<0>(stack.back()), reducedNT.getShortDescription().c_str());
				}
			}

			outputParseTree.rootIndex = nodeIndex;
			stack.push_back({ table.GOTO[{std::get<0>(stack.back()), reducedNT }], reducedNT, nodeIndex });
		}

		stepI++;
		printAnalysisStep(logger, stepI, stack);
	}
	logger.printLogToStdout = true;
	outputParseTree.print(logger);
	logger.printLogToStdout = backup;
}

template<class Out>
inline void LR1Parser::printAnalysisStep(Out & out, Index stepI, const std::vector<std::tuple<StateIndex, StringifiedGrammarSymbol, Index>>& stack)
{
	out << std::resetiosflags(std::ios::right) << std::setiosflags(std::ios::left);
	std::ostringstream stackStateStream;
	std::ostringstream stackSymbolStream;

	for (auto &p : stack)
	{
		if (std::get<0>(p) == (std::numeric_limits<AkaneLang::StateIndex>::max)() - 1)
			stackStateStream << "<End>|";
		else
			stackStateStream << std::get<0>(p) << "|";
		if (std::get<1>(p) == AkaneLang::StringifiedGrammarSymbol::epsilon())
			stackSymbolStream << "ε" << "|";
		else
			stackSymbolStream << std::get<1>(p).getShortDescription() << "|";
	}

	std::string stackStateStr = stackStateStream.str();
	std::string stackSymbolStr = stackSymbolStream.str();
	if (stackStateStr.size())
	{
		stackStateStr.pop_back();
		stackSymbolStr.pop_back();
	}

	out << std::setw(5) << stepI << std::setw(2) << "   " << std::setw(40) << stackStateStr << std::setw(4) << "   " << std::setw(60) << stackSymbolStr << std::setw(4) << "   ";

	for (Index i = inputTokenizedLetterGenerator.i; i != inputTokenizedLetterGenerator.tokLetters.size(); i++)
	{
		if (i != inputTokenizedLetterGenerator.i)
			out << "|";
		out << inputTokenizedLetterGenerator.tokLetters[i].getShortDescription();
	}

	out << std::endl;
}


template void LRAnalysisTable::print<std::ostream>(std::ostream &) const;
template void LRAnalysisTable::print<AkaneUtils::Logger>(AkaneUtils::Logger &) const;