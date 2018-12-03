
namespace AkaneLang
{
	struct LetterToken;

	struct GrammarSymbol : public virtual Letter
	{
		virtual std::string getUniqueName() const = 0;
		virtual std::string getSymbolDescription() const = 0;
		//virtual void setName(const std::string &_name) = 0;
		virtual bool operator<(const Letter &l) const { return getUniqueName() < dynamic_cast<const GrammarSymbol &>(l).getUniqueName(); }
		virtual bool operator>(const Letter &l) const { return getUniqueName() > dynamic_cast<const GrammarSymbol &>(l).getUniqueName(); }
		virtual bool operator<=(const Letter &l) const { return getUniqueName() <= dynamic_cast<const GrammarSymbol &>(l).getUniqueName(); }
		virtual bool operator>=(const Letter &l) const { return getUniqueName() >= dynamic_cast<const GrammarSymbol &>(l).getUniqueName(); }
		virtual bool operator==(const Letter &l) const { return getUniqueName() == dynamic_cast<const GrammarSymbol &>(l).getUniqueName(); }
		virtual bool operator!=(const Letter &l) const { return getUniqueName() != dynamic_cast<const GrammarSymbol &>(l).getUniqueName(); }

		virtual bool isTerminal() const = 0;

		virtual GrammarSymbol *duplicate_freeNeeded() const = 0;

		virtual ~GrammarSymbol() {};
	};

	struct NamedGrammarSymbol : public virtual GrammarSymbol
	{
		std::string symbolName;

		static const NamedGrammarSymbol &epsilon() { static NamedGrammarSymbol epsilon("");  return epsilon; }
		static const NamedGrammarSymbol &elseLetter() { static NamedGrammarSymbol elseSym("\1");  return elseSym; }
		NamedGrammarSymbol() : symbolName("(invalid)") {}
		NamedGrammarSymbol(const NamedGrammarSymbol &s2) : symbolName(s2.symbolName) {}
		NamedGrammarSymbol(const std::string &_symbolName) : symbolName(_symbolName) {}
		virtual std::string getUniqueName() const override { return symbolName; }
		virtual std::string getSymbolDescription() const { return AkaneLang::escape(getUniqueName()); }
		virtual std::string getLetterDescription() const { return AkaneLang::escape(getUniqueName()); }
		virtual std::string getLongDescription() const { return std::string("[ ") + AkaneLang::escape(getUniqueName()) + std::string(" ]"); }
		//virtual void setName(const std::string &_name) override { terminalName = _name; }
		virtual bool isTerminal() const { throw AkaneRuntimeException("你不应该调用 isTerminal()"); }
		virtual GrammarSymbol *duplicate_freeNeeded() const { throw AkaneRuntimeException("你不应该调用 duplicate_freeNeeded()"); }

		NamedGrammarSymbol(const LetterToken &lt);

		virtual ~NamedGrammarSymbol() {};
	};

	struct Terminal : public virtual GrammarSymbol
	{
		virtual bool isTerminal() const
		{
			return true;
		}
		virtual ~Terminal() {}
	};

	struct SimpleTerminal : public NamedGrammarSymbol, public Terminal
	{
		virtual bool isTerminal() const
		{
			return Terminal::isTerminal();
		}

		static const SimpleTerminal &epsilon() { static SimpleTerminal epsilonTerminal("");  return epsilonTerminal; }
		static const SimpleTerminal &eof() { static SimpleTerminal eofTerminal(zero);  return eofTerminal; }
		SimpleTerminal(const SimpleTerminal &st2) : NamedGrammarSymbol(st2) {}
		SimpleTerminal(const std::string &_terminalName) : NamedGrammarSymbol(_terminalName) {}

		virtual SimpleTerminal *duplicate_freeNeeded() const override { return new SimpleTerminal(*this); }
	};

	struct NonTerminal : public virtual GrammarSymbol
	{
		virtual bool isTerminal() const
		{
			return false;
		}
		virtual ~NonTerminal() {}
	};

	struct SimpleNonTerminal : public NamedGrammarSymbol, public NonTerminal
	{
		virtual bool isTerminal() const
		{
			return NonTerminal::isTerminal();
		}

		SimpleNonTerminal(const SimpleNonTerminal &snt2) : NamedGrammarSymbol(snt2) {}
		SimpleNonTerminal(const std::string &_nonTerminalName) : NamedGrammarSymbol(_nonTerminalName) {}

		virtual SimpleNonTerminal *duplicate_freeNeeded() const override { return new SimpleNonTerminal(*this); }
	};


	typedef size_t SymbolIndex;
	constexpr SymbolIndex epsilonIndex = (std::numeric_limits<SymbolIndex>::max)();

	class Grammar;

	// 产生式
	class Production
	{
		friend class Grammar;
	public:
		Grammar *grammarP;
		Index indexInGrammar;

		std::vector<SymbolIndex> leftIndices;
		std::vector<SymbolIndex> rightIndices;

		std::string getDescription();

		//private:
		Production(Grammar *_grammarP, std::vector<SymbolIndex> _leftIndices, std::vector<SymbolIndex> _rightIndices, Index _indexInGrammar) : grammarP(_grammarP), leftIndices(_leftIndices), rightIndices(_rightIndices), indexInGrammar(_indexInGrammar)
		{}
	};

	// LR(1) 项目
	class LRItem
	{
	public:
		Production *productionP;

		size_t dotPosition;
		SymbolIndex lookAheadIndex;

		std::string getDescription();
		
		bool isReducing()
		{
			return dotPosition == productionP->rightIndices.size();
		}

		LRItem(Production &_production, size_t _dotPosition, SymbolIndex _lookAheadIndex) : productionP(&_production), dotPosition(_dotPosition), lookAheadIndex(_lookAheadIndex) {}

		LRItem(const LRItem &lr2) : productionP(lr2.productionP), dotPosition(lr2.dotPosition), lookAheadIndex(lr2.lookAheadIndex) {}

		bool operator==(const LRItem &lri);

		LRItem &operator=(const LRItem &lri);
		
	};

	// 文法
	class Grammar
	{
	public:

		std::string name;
		std::string arrowStr;

		SymbolIndex startSymbolIndex;
		SymbolIndex eofSymbolIndex;

		std::vector<GrammarSymbol *> symbols;
		std::vector<Production> productions;
	private:
		std::vector<std::set<SymbolIndex>> FIRST;
	public:
		Grammar()
		{
			char tmpBuf[40] = { 0 };
			sprintf(tmpBuf, "文法 %x", static_cast<unsigned>(reinterpret_cast<unsigned long long>(this)));
			name = tmpBuf;
		}

		void setStartSymbol(const NonTerminal &nt)
		{
			auto symFindIt = std::find_if(symbols.begin(), symbols.end(), [&nt](GrammarSymbol *symP) -> bool {return *symP == nt; const Letter &a = nt; });

			if (symFindIt == symbols.end())
			{
				throw AkaneInputValueException("输入的 %s 没有与之匹配的文法符号.", nt.getSymbolDescription().c_str());
			}

			startSymbolIndex = std::distance(symbols.begin(), symFindIt);
		}

		void addEOFSymbol()
		{
			eofSymbolIndex = symbols.size();
			symbols.push_back(SimpleTerminal::eof().duplicate_freeNeeded());
		}

		void addSymbol(const GrammarSymbol &_symbol)
		{
			symbols.push_back(_symbol.duplicate_freeNeeded());
		}

		void addProduction(std::vector<std::string> _left, std::vector<std::string> _right)
		{
			// Left
			std::vector<SymbolIndex> leftIndices;

			for (auto &symStr : _left)
			{
				auto symFindIt = std::find_if(symbols.begin(), symbols.end(), [&symStr](GrammarSymbol *symP) -> bool {return symP->getSymbolDescription() == symStr; });
				if (symFindIt == symbols.end())
				{
					throw AkaneInputValueException("输入的 %s 没有与之匹配的文法符号.", symStr.c_str());
				}

				leftIndices.push_back(std::distance(symbols.begin(), symFindIt));
			}

			// Right
			std::vector<SymbolIndex> rightIndices;

			for (auto &symStr : _right)
			{
				auto symFindIt = std::find_if(symbols.begin(), symbols.end(), [&symStr](GrammarSymbol *symP) -> bool {return symP->getSymbolDescription() == symStr; });
				if (symFindIt == symbols.end())
				{
					throw AkaneInputValueException("输入的 %s 没有与之匹配的文法符号.", symStr.c_str());
				}

				rightIndices.push_back(std::distance(symbols.begin(), symFindIt));
			}

			productions.push_back(Production(this, leftIndices, rightIndices, productions.size()));
		}

		void generateFIRST()
		{
			FIRST.clear();
			bool somethingChanged = true;
			while (somethingChanged)
			{
				somethingChanged = false;
				for (SymbolIndex symI = 0; symI < symbols.size(); symI++)
				{
					auto &sym = *(symbols[symI]);
					if (FIRST.size() == symI)
						FIRST.push_back(std::set<SymbolIndex>{});
					auto &thisFIRST = FIRST[symI];
					size_t prevSize = thisFIRST.size();

					if (sym.isTerminal())
					{
						thisFIRST.insert(symI);
					}
					else
					{
						for (auto &prod : productions)
						{
							if (prod.leftIndices.size() == 1 && prod.leftIndices[0] == symI)
							{
								// X->...
								// 这两个分支应该可以用某种巧妙的方式重写, 组合起来
								if (prod.rightIndices.size() == 0)
								{
									// X-><epsilon>
									thisFIRST.insert(epsilonIndex);
								}
								else
								{
									SymbolIndex firstSymIndex = prod.rightIndices[0];
									auto &firstSym = *(symbols[firstSymIndex]);
									if (firstSym.isTerminal())
									{
										// X->a...
										thisFIRST.insert(firstSymIndex);
									}
									else
									{
										// X->Y...
										
										// 得到 FIRST(Y) - {<epsilon>}
										auto tmpSet = FIRST.size() <= firstSymIndex ? std::set<SymbolIndex>{} : FIRST[firstSymIndex];
										auto findEpsilonIt = tmpSet.find(epsilonIndex);
										bool haveEpsilon;
										if (haveEpsilon = (findEpsilonIt != tmpSet.end()))
										{
											tmpSet.erase(findEpsilonIt);
										}
										thisFIRST.insert(tmpSet.begin(), tmpSet.end());

										// 若 FIRST(Y) 含 <epsilon> 要将后续的 FIRST 加入
										if (haveEpsilon)
										{
											auto rightSymIIt = prod.rightIndices.begin() + 1;
											for (; rightSymIIt != prod.rightIndices.end(); rightSymIIt++)
											{
												SymbolIndex rightSymI = *rightSymIIt;
												if (symbols[rightSymI]->isTerminal())
												{
													thisFIRST.insert(firstSymIndex);
													// Y[rightSymIIt] 是终结符, 应该跳出循环
													break;
												}

												tmpSet = FIRST[firstSymIndex];
												findEpsilonIt = tmpSet.find(epsilonIndex);
												if (haveEpsilon = (findEpsilonIt != tmpSet.end()))
												{
													tmpSet.erase(findEpsilonIt);
												}
												thisFIRST.insert(tmpSet.begin(), tmpSet.end());
												if (haveEpsilon)
												{
													// Y[rightSymIIt] 没有 <epsilon>, 应该跳出循环
													break;
												}
											}

											if (rightSymIIt == prod.rightIndices.end())
											{
												// 没有跳出循环, 一路都遇到 <epsilon>
												thisFIRST.insert(epsilonIndex);
											}
										}
									}
								}
							}
						}
					}
					if (thisFIRST.size() != prevSize)
						somethingChanged = true;
				}
			}

			return;
		}

		/*
		std::set<SymbolIndex> getFIRST(SymbolIndex _letterIndex)
		{
			if (FIRST.size() == 0)
				generateFIRST();

			if (_letterIndex == epsilonIndex)
			return result;
		}*/

		// 获取一序列文法符号的 FIRST
		std::set<SymbolIndex> getFIRST(std::vector<SymbolIndex> _letterIndices)
		{
			if (FIRST.size() == 0)
				generateFIRST();

			std::set<SymbolIndex> result;
			auto symIIt = _letterIndices.begin();
			for (; symIIt != _letterIndices.end(); symIIt++)
			{
				SymbolIndex symI = *symIIt;
				std::set<SymbolIndex> tmpSet(FIRST[symI]);
				auto findEpsilonIt = tmpSet.find(epsilonIndex);
				bool haveEpsilon;
				if (haveEpsilon = (findEpsilonIt != tmpSet.end()))
				{
					// have epsilon
					tmpSet.erase(findEpsilonIt);
				}
				result.insert(tmpSet.begin(), tmpSet.end());

				if (!haveEpsilon)
				{
					break;
				}
			}

			if (symIIt == _letterIndices.end())
			{
				result.insert(epsilonIndex);
			}

			return result;
		}

		template <class Out>
		void print(Out &out)
		{
			out << "==== 打印文法 " << name << " ====" << std::endl;
			out << std::endl;
			out << "符号表: " << std::endl;
			out << std::endl;
			out << "终结符: ";
			for (auto symP : symbols)
			{
				if (symP->isTerminal() && *symP != SimpleTerminal::eof())
				{
					out << symP->getSymbolDescription() << " ";
				}
			}
			out << std::endl;
			out << "非终结符: ";
			for (auto symP : symbols)
			{
				if (!(symP->isTerminal()))
				{
					out << symP->getSymbolDescription() << " ";
				}
			}
			out << std::endl;
			out << std::endl;

			out << "起始符号: " << symbols[startSymbolIndex]->getSymbolDescription() << std::endl << std::endl;

			out << "生成式:" << std::endl << std::endl;

			for (auto &p : productions)
			{
				out << p.getDescription() << std::endl;
			}

			if (FIRST.size())
			{
				out << std::endl;
				out << "FIRST 集合: " << std::endl;
				for (SymbolIndex symI = 0; symI < symbols.size(); symI++)
				{
					out << "FIRST(" << symbols[symI]->getSymbolDescription() << ") = { ";
					for (auto symI2 : FIRST[symI])
					{
						if (symI2 == epsilonIndex)
							out << "ε " << ", ";
						else
							out << symbols[symI2]->getSymbolDescription() << ", ";
					}
					out << "}" << std::endl;
				}
			}
		}

		~Grammar()
		{
			for (auto symP : symbols)
			{
				delete symP;
			}
		}
	};
}