#pragma once
#include "..\akane-lex\akane-lex.h"
#include "parse-tree.h"
DEFINE_AKANE_EXCEPTION(Parse)


namespace AkaneParse
{
	const char *actionText[2] = {
		"移进",
		"归约"
	};

	enum LRAnalysisAction
	{
		shift,
		reduce
	};

	class LRAnalysisTable
	{
	public:
		size_t statesSize;
		size_t terminalsSize;
		size_t nonTerminalsSize;
		AkaneLang::Grammar *relatedGrammar;
		std::map<std::pair<AkaneLang::StateIndex, AkaneLang::SymbolIndex>, std::pair<LRAnalysisAction, Index>> ACTION;
		std::map<std::pair<AkaneLang::StateIndex, AkaneLang::SymbolIndex>, AkaneLang::StateIndex> GOTO;
		std::map<AkaneLang::NamedGrammarSymbol, AkaneLang::SymbolIndex> terminalToIndex;

		LRAnalysisTable() : statesSize(0), terminalsSize(0), nonTerminalsSize(0), relatedGrammar(nullptr)
		{
		}

		void mapTerminalToIndex()
		{
			terminalToIndex.clear();
			for (Index i = 0; i < relatedGrammar->symbols.size(); i++)
			{
				auto &sym = dynamic_cast<AkaneLang::NamedGrammarSymbol &>(*(relatedGrammar->symbols[i]));
				terminalToIndex[sym] = i;
			}
		}

		void insertACTION(const std::pair<AkaneLang::StateIndex, AkaneLang::SymbolIndex> &left, const std::pair<LRAnalysisAction, Index> &right)
		{
			auto &stateI = left.first;
			auto &symI = left.second;
			auto &action = right.first;
			auto &nextStateIndex = right.second;

			const std::string &currSymStr = relatedGrammar->symbols[symI]->getSymbolDescription();

			if (ACTION.count({ stateI, symI }))
			{
				auto &a = ACTION[{stateI, symI}];
				const char *cStr = currSymStr.c_str();
				std::stringstream tmpStream1;
				std::stringstream tmpStream2;

				if (a.first == shift)
					tmpStream1 << a.second;
				else
					tmpStream1 << relatedGrammar->productions[a.second].getDescription();

				if (right.first == shift)
					tmpStream2 << right.second;
				else
					tmpStream2 << relatedGrammar->productions[right.second].getDescription();

				throw AkaneParseException("ACTION[%u, %s] = %s:%s 与 ACTION[%u, %s] = %s:%s 冲突, 该文法不是 LR(1) 的.", stateI, cStr, actionText[a.first], tmpStream1.str().c_str(), stateI, cStr, actionText[action], tmpStream2.str().c_str());
			}
			ACTION[left] = right;
		}

		void insertGOTO(const std::pair<AkaneLang::StateIndex, AkaneLang::SymbolIndex> &pair, AkaneLang::StateIndex nextStateIndex)
		{
			auto &stateI = pair.first;
			auto &symI = pair.second;
			const std::string &currSymStr = relatedGrammar->symbols[symI]->getSymbolDescription();
			if (GOTO.count({ stateI, symI }))
			{
				auto g = GOTO[{stateI, symI}];
				const char *cStr = currSymStr.c_str();
				throw AkaneParseException("GOTO[%u, %s] = %u 与 GOTO[%u, %s] = %u 冲突.", stateI, cStr, g, stateI, cStr, nextStateIndex);
			}
			GOTO[pair] = nextStateIndex;
		}

		template <class Out>
		void print(Out &out)
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

			for (auto &sym : relatedGrammar->symbols)
			{
				AkaneLang::NamedGrammarSymbol &symN = dynamic_cast<AkaneLang::NamedGrammarSymbol &>(*sym);
				if (symN.isTerminal())
				{
					out << std::setw(12) << symN.getSymbolDescription();
				}
			}
			for (auto &sym : relatedGrammar->symbols)
			{
				AkaneLang::NamedGrammarSymbol &symN = dynamic_cast<AkaneLang::NamedGrammarSymbol &>(*sym);
				if (!symN.isTerminal())
				{
					out << std::setw(12) << symN.getSymbolDescription();
				}
			}
			out << std::endl;

			for (AkaneLang::StateIndex i = 0; i < statesSize; i++)
			{
				out << std::resetiosflags(std::ios::left);
				out << std::setiosflags(std::ios::right);
				out << "|" << std::setw(6) << i << "|";
				out << std::resetiosflags(std::ios::right);
				out << std::setiosflags(std::ios::left);
				for (Index j = 0; j < relatedGrammar->symbols.size(); j++)
				{
					const AkaneLang::NamedGrammarSymbol &symN = dynamic_cast<const AkaneLang::NamedGrammarSymbol &>(*relatedGrammar->symbols[j]);
					if (symN.isTerminal())
					{
						if (ACTION.count({ i, j }))
						{
							auto &p = ACTION.at({ i, j });
							std::ostringstream os;
							os << actionText[p.first] << ":";

							//if (p.first == shift)
							os << p.second;
							//else
								//os << relatedGrammar->productions[p.second].getDescription();
							out << std::setw(12) << os.str();
						}
						else
						{
							out << std::setw(12) << "x";
						}
					}
				}
				for (Index j = 0; j < relatedGrammar->symbols.size(); j++)
				{
					const AkaneLang::NamedGrammarSymbol &symN = dynamic_cast<const AkaneLang::NamedGrammarSymbol &>(*relatedGrammar->symbols[j]);
					if (!symN.isTerminal())
					{
						if (GOTO.count({ i, j }))
						{
							auto &nsi = GOTO.at({ i, j });

							out << std::setw(12) << nsi;
						}
						else
						{
							out << std::setw(12) << "x";
						}
					}
				}
				out << std::endl;
			}
		}
			
	};

	class Parser
	{
	public:
		AkaneLang::LetterTokenGenerator &ltg;
		AkaneLang::Grammar grammar;

		std::vector<AkaneLang::LRItem> lrItems;

		LRAnalysisTable table;
		ParseTree tree;

		Parser(AkaneLang::LetterTokenGenerator &_ltg) : ltg(_ltg)
		{}

		void insertNewNonRedundantState(std::vector<AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>> &states, Index i, AkaneLang::AutomataStateTag nextStateTag, const AkaneLang::NamedGrammarSymbol &currSymbol, AkaneLang::LRItem &newItem)
		{

			auto findSameItemIt = std::find(lrItems.begin(), lrItems.end(), newItem);

			auto findSameStateIt = std::find_if(states.begin(), states.end(), [this, &newItem, nextStateTag](const AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol> &s) -> bool {
				if (s.itemIndices.size() == 0)
					return false;
				return lrItems[(*s.itemIndices.begin())] == newItem && s.tag == nextStateTag;
			});


			if (findSameItemIt == lrItems.end())
			{
				lrItems.push_back(newItem);
				states.push_back(AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>(&lrItems, { lrItems.size() - 1 }, nextStateTag));

				states[i].delta[currSymbol].insert(states.size() - 1);
			}
			else if (findSameStateIt == states.end())
			{
				Index sameItemIndex = std::distance(lrItems.begin(), findSameItemIt);
				states.push_back(AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>(&lrItems, { sameItemIndex }, nextStateTag));

				states[i].delta[currSymbol].insert(states.size() - 1);
			}
			else
			{
				states[i].delta[currSymbol].insert(std::distance(states.begin(), findSameStateIt));
			}
		}

		std::vector<AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>> makeItemStates()
		{
			std::vector<AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>> states;

			// 一个初态
			states.push_back(AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>(&lrItems, std::set<Index>{}, AkaneLang::AutomataStateTag::notAccepted));

			//std::set<Index> previousStateOfAccepted;

			// 找出开始符号对应的产生式
			for (auto &prod : grammar.productions)
			{
				if (prod.leftIndices.size() == 1 && prod.leftIndices[0] == grammar.startSymbolIndex)
				{
					AkaneLang::LRItem item = AkaneLang::LRItem(prod, 0, grammar.eofSymbolIndex);
					lrItems.push_back(item);
					states.push_back(AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>(&lrItems,{ lrItems.size() - 1 }, AkaneLang::AutomataStateTag::notAccepted));
					states[0].description = "(初始空)";
					states[0].delta[AkaneLang::SimpleTerminal::epsilon()].insert(states.size() - 1);

					//previousStateOfAccepted.insert(states.size() - 1);
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

				auto &currSymbol = dynamic_cast<const AkaneLang::NamedGrammarSymbol &>(*currProd.grammarP->symbols[currProd.rightIndices[currItem.dotPosition]]);

				// X -> AB.CD

				// 加入 Item X -> ABC.D 并添加 delta 项

				AkaneLang::LRItem shiftItem = AkaneLang::LRItem(currItem);
				shiftItem.dotPosition++;

				AkaneLang::AutomataStateTag nextStateTag = AkaneLang::AutomataStateTag::notAccepted;
				if (shiftItem.dotPosition == shiftItem.productionP->rightIndices.size() && shiftItem.lookAheadIndex == grammar.eofSymbolIndex)
				{
					nextStateTag = AkaneLang::AutomataStateTag::accepted;
				}

				insertNewNonRedundantState(states, i, nextStateTag, currSymbol, shiftItem);

				if (currSymbol.isTerminal())
				{
					// X -> AB.cD  =>  X -> ABc.D 已经结束
					continue;
				}
				else
				{
					// X -> ab.CdeF (g) => C -> ....

					std::vector<AkaneLang::SymbolIndex> symIndicesToGetFIRST;

					for (size_t pos = currItem.dotPosition + 1; pos < currProd.rightIndices.size(); pos++)
					{
						symIndicesToGetFIRST.push_back(currProd.rightIndices[pos]);
					}

					symIndicesToGetFIRST.push_back(currItem.lookAheadIndex);

					std::set<AkaneLang::SymbolIndex> FIRSTofRemainingSymbols = grammar.getFIRST(symIndicesToGetFIRST);

					for (auto &prod : grammar.productions)
					{
						if (prod.leftIndices.size() == 1 && prod.leftIndices[0] == currProd.rightIndices[currItem.dotPosition])
						{
							// C -> .xyz (one of FIRST(deFg))

							// 对于 FIRST(deFg) 的所有项, 都要检查并插入一遍
							for (auto oneOfFIRST : FIRSTofRemainingSymbols)
							{
								AkaneLang::LRItem newItem = AkaneLang::LRItem(prod, 0, oneOfFIRST);

								insertNewNonRedundantState(states, i, AkaneLang::AutomataStateTag::notAccepted, AkaneLang::SimpleTerminal::epsilon(), newItem);
							}
						}
					}
				}
				
			}

			return states;
		}

		void makeGrammar(std::istream &is)
		{
			grammar.addEOFSymbol();
			is >> grammar.arrowStr;


			// 终结符
			size_t Tnum;
			is >> Tnum;
			for (size_t i = 0; i < Tnum; i++)
			{
				std::string tStr;
				is >> tStr;
				grammar.addSymbol(AkaneLang::SimpleTerminal(tStr));
			}

			// 非终结符
			size_t NTnum;
			is >> NTnum;
			for (size_t i = 0; i < NTnum; i++)
			{
				std::string ntStr;
				is >> ntStr;
				grammar.addSymbol(AkaneLang::SimpleNonTerminal(ntStr));
			}

			// 产生式
			is >> std::ws;
			while (true)
			{
				std::string thisLine;
				std::getline(is, thisLine);
				std::istringstream thisLineStream(thisLine);

				size_t symbolCount = 0;
				std::string currSymbol;
				std::vector<std::string> left;
				std::vector<std::string> right;
				std::vector<std::string> *side = &left;

				thisLineStream >> std::ws;

				while (true)
				{
					thisLineStream >> std::ws;
					if (thisLineStream.eof())
						if (symbolCount == 0)
							break;
						else if (side == &left)
							throw AkaneInputValueException("文法输入错误: %s %s -> %s %s...", left.size() > 0 ? left[0].c_str() : "(empty)", left.size() > 1 ? left[1].c_str() : "(empty)", right.size() > 0 ? right[0].c_str() : "(empty)", right.size() > 1 ? right[1].c_str() : "(empty)");
						else
							break;

					thisLineStream >> currSymbol;
					
					if (currSymbol == grammar.arrowStr)
					{
						if (side == &right)
							throw AkaneInputValueException("文法出现了两次以上箭头: %s %s -> %s %s...", left.size() > 0 ? left[0].c_str() : "(empty)", left.size() > 1 ? left[1].c_str() : "(empty)", right.size() > 0 ? right[0].c_str() : "(empty)", right.size() > 1 ? right[1].c_str() : "(empty)");
						
						side = &right;
						if (symbolCount == 0)
						{
							// 开始就是箭头, 是退出的标志
							symbolCount = (std::numeric_limits<size_t>::max)();
							break;
						}
						symbolCount = 0;
						continue;
					}

					side->push_back(currSymbol);
					symbolCount++;
				}

				if (symbolCount == (std::numeric_limits<size_t>::max)())
				{
					// 开始遇到箭头, 整个文法结束
					break;
				}

				if (symbolCount == 0)
				{
					if (side == &left)
					{
						// 本行空
						continue;
					}
				}

				grammar.addProduction(left, right);
			}

			std::string startSymbol;
			is >> startSymbol;

			grammar.setStartSymbol(AkaneLang::SimpleNonTerminal(startSymbol));
		}

		AkaneLang::DFA<AkaneLang::NamedGrammarSymbol, AkaneLang::ItemState> makeDFA()
		{

			std::vector<AkaneLang::NamedGrammarSymbol> alphabet;
			for (auto symP : grammar.symbols)
			{
				if (*symP == AkaneLang::SimpleTerminal::eof())
					continue;

				alphabet.push_back(dynamic_cast<const AkaneLang::NamedGrammarSymbol &>(*symP));
			}

			std::vector<AkaneLang::ItemState<AkaneLang::NamedGrammarSymbol>> states = makeItemStates();
			
			// 造 NFA

			AkaneLang::NFA<AkaneLang::NamedGrammarSymbol, AkaneLang::ItemState> nfa(alphabet, std::map<AkaneLang::NamedGrammarSymbol, AkaneLang::NamedGrammarSymbol>{}, &this->ltg);

			nfa.loadStates(states);
			nfa.setInitialStateIndex(0);
			nfa.isNotMatchedLettersMappedToElse = false;

			nfa.print(logger);

			// 造 DFA
			AkaneLang::DFA<AkaneLang::NamedGrammarSymbol, AkaneLang::ItemState> dfa(nfa);
			dfa.validate();
			dfa.print(logger);
			return dfa;
		}

		void makeTable(const AkaneLang::DFA<AkaneLang::NamedGrammarSymbol, AkaneLang::ItemState> &dfa)
		{
			// 初始化
			table.relatedGrammar = &grammar;
			table.statesSize = dfa.states.size() - 1;	// 剪掉 NFA 转 DFA 额外加的一个陷阱状态
			
			for (Index symI = 0; symI < grammar.symbols.size(); symI++)
			{
				auto &currSym = dynamic_cast<AkaneLang::NamedGrammarSymbol &>(*grammar.symbols[symI]);

				if (currSym.isTerminal())
				{
					table.terminalsSize++;
				}
				else
				{
					table.nonTerminalsSize++;
				}
			}

			table.mapTerminalToIndex();


			for (AkaneLang::StateIndex stateI = 0; stateI < dfa.states.size(); stateI++)
			{
				auto &currState = dfa.states[stateI];
				for (Index symI = 0; symI < grammar.symbols.size(); symI++)
				{
					const AkaneLang::NamedGrammarSymbol &currSym = dynamic_cast<const AkaneLang::NamedGrammarSymbol &>(*grammar.symbols[symI]);
					if (currState.delta.count(currSym) == 0)
						continue;
					AkaneLang::StateIndex nextStateIndex = currState.delta.at(currSym).begin().operator*();
					auto &nextState = dfa.states[nextStateIndex];

					if (currSym.isTerminal())
					{
						if (nextState.itemIndices.size() == 0)
							continue;

						table.insertACTION({ stateI, symI }, { shift, nextStateIndex });
					}
					else
					{
						if (nextState.itemIndices.size() == 0)
							continue;

						table.insertGOTO({ stateI, symI }, nextStateIndex);
					}
				}

				for (auto lrII : currState.itemIndices)
				{
					if (lrItems[lrII].isReducing())
					{
						table.insertACTION({ stateI, lrItems[lrII].lookAheadIndex }, { reduce, lrItems[lrII].productionP->indexInGrammar });
					}
				}
			}
		}

		template <class Out>
		void printAnalysisStep(Out &out, Index stepI, const std::vector<std::tuple<AkaneLang::StateIndex, AkaneLang::SymbolIndex, Index>> &stack)
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
				if (std::get<1>(p) == AkaneLang::epsilonIndex)
					stackSymbolStream << "ε" << "|";
				else
					stackSymbolStream << grammar.symbols[std::get<1>(p)]->getSymbolDescription() << "|";
			}

			std::string stackStateStr = stackStateStream.str();
			std::string stackSymbolStr = stackSymbolStream.str();
			if (stackStateStr.size())
			{
				stackStateStr.pop_back();
				stackSymbolStr.pop_back();
			}

			out << std::setw(5) << stepI << std::setw(40) << "   " << stackStateStr << std::setw(60) << "   " << stackSymbolStr;

			for (auto p = ltg.p; p != ltg.v.end(); p++)
			{
				if (p != ltg.p)
					out << "|";
				out << p->getSymbolDescription();
			}

			out << std::endl;
		}

		void analyze()
		{
			bool backup = logger.printLogToStdout;
			static std::ifstream f("grammar.txt", std::ios::in);
			makeGrammar(f);
			auto dfa = makeDFA();
			makeTable(dfa);

			grammar.print(logger);
			table.print(logger);

			logger << "==== LR(1) 分析开始 ====" << std::endl;

			std::vector<std::tuple<AkaneLang::StateIndex, AkaneLang::SymbolIndex, Index>> stack;
			size_t stepI = 0;
			stack.push_back({ dfa.initialStateIndex, AkaneLang::epsilonIndex, -1 });
			printAnalysisStep(logger, stepI, stack);

			while (true)
			{
				const AkaneLang::LetterToken &lt = ltg.peek_freeNeeded();

				AkaneLang::SymbolIndex mappedIndex = table.terminalToIndex.at(lt);

				if (table.ACTION.count({ std::get<0>(stack.back()), mappedIndex }) == 0)
				{
					throw AkaneParseException("不符合语法.");
				}
				auto &action = table.ACTION.at({ std::get<0>(stack.back()), mappedIndex });

				if (action.first == shift)
				{
					Index nodeIndex = tree.addNode(lt);

					stack.push_back({ action.second, mappedIndex, nodeIndex });

					const AkaneLang::LetterToken &lt2 = ltg.next_freeNeeded();
					delete &lt2;
				}
				else
				{
					AkaneLang::SymbolIndex reducedNTIndex = grammar.productions[action.second].leftIndices[0];
					Index nodeIndex = tree.addNode(*grammar.symbols[reducedNTIndex]);
					for (Index k = 0; k < grammar.productions[action.second].rightIndices.size(); k++)
					{
						tree.nodes[nodeIndex].children.insert(tree.nodes[nodeIndex].children.begin(), std::get<2>(stack.back()));
						stack.pop_back();
					}
					AkaneLang::StateIndex savedPrevState = std::get<0>(stack.back());
					if (table.GOTO.count({ std::get<0>(stack.back()), reducedNTIndex }) == 0)
					{
						if (lt == AkaneLang::LetterToken::eof() && stack.size() == 1 && reducedNTIndex == grammar.startSymbolIndex)
						{
							tree.rootIndex = nodeIndex;
							stack.push_back({ (std::numeric_limits<AkaneLang::StateIndex>::max)() - 1, reducedNTIndex, nodeIndex });
							stepI++;
							printAnalysisStep(logger, stepI, stack);

							logger << "GOTO[" << savedPrevState << ", " << grammar.symbols[reducedNTIndex]->getSymbolDescription() << "] 查询失败而正常退出." << std::endl;
							break;
						}
						else
						{
							throw AkaneParseException("GOTO[%u, %s] 不存在.", std::get<0>(stack.back()), grammar.symbols[reducedNTIndex]->getSymbolDescription().c_str());
						}
						//throw AkaneParseException("GOTO[%u, %s] 不存在.", stack.back().first, grammar.symbols[reducedNTIndex]->getSymbolDescription().c_str());
					}

					tree.rootIndex = nodeIndex;
					stack.push_back({ table.GOTO[{std::get<0>(stack.back()), reducedNTIndex }], reducedNTIndex, nodeIndex });
				}

				delete &lt;

				stepI++;
				printAnalysisStep(logger, stepI, stack);
			}
			logger.printLogToStdout = true;
			tree.print(logger);
			logger.printLogToStdout = backup;
		}
	};
}