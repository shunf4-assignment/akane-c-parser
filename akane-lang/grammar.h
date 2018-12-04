
namespace AkaneLang
{
	typedef size_t SymbolIndex;
	constexpr SymbolIndex epsilonIndex = (std::numeric_limits<SymbolIndex>::max)();

	class Grammar;

	struct GrammarSymbol : public virtual Letter
	{
		virtual bool isTerminal() const = 0;
		virtual ~GrammarSymbol();

		virtual GrammarSymbol *duplicate_freeNeeded() const override = 0;
	};

	/* 本来这样就是完全合法的: struct StringifiedGrammarSymbol : public virtual GrammarSymbol, public StringifiedLetter
		但由于 MSVC++ 2010 以上的一个 bug : https://stackoverflow.com/questions/3592648/c-weird-diamond-inheritance-issue
		 https://connect.microsoft.com/VisualStudio/feedback/details/590625/visual-c-incorrectly-reports-ambiguity-when-covariance-is-used-with-virtual-inheritance
		需要写成这样
	*/
	struct StringifiedGrammarSymbol : public virtual Letter, public virtual GrammarSymbol, public StringifiedLetter
	{
		static const StringifiedGrammarSymbol &epsilon();
		StringifiedGrammarSymbol();
		StringifiedGrammarSymbol(const StringifiedGrammarSymbol &s2);
		StringifiedGrammarSymbol(const std::string &_name);
		StringifiedGrammarSymbol(const TokenizedLetter &lt);

		virtual bool isTerminal() const;
		virtual StringifiedGrammarSymbol *duplicate_freeNeeded() const override
		{
			return new StringifiedGrammarSymbol(*this);
		}

		virtual ~StringifiedGrammarSymbol();
	};

	struct Terminal : public virtual GrammarSymbol
	{
		virtual bool isTerminal() const;

		virtual ~Terminal();
	};

	struct StringifiedTerminal : public StringifiedGrammarSymbol, public Terminal
	{
		virtual bool isTerminal() const;

		static const StringifiedTerminal &epsilon();
		static const StringifiedTerminal &eof();
		StringifiedTerminal(const StringifiedTerminal &st2);
		StringifiedTerminal(const std::string &_terminalName); 

		virtual StringifiedTerminal *duplicate_freeNeeded() const override;
	};

	struct NonTerminal : public virtual GrammarSymbol
	{
		virtual bool isTerminal() const
		{
			return false;
		}
		virtual ~NonTerminal() {}
	};

	struct StringifiedNonTerminal : public StringifiedGrammarSymbol, public NonTerminal
	{
		virtual bool isTerminal() const override;

		static const StringifiedNonTerminal &epsilon() { static StringifiedNonTerminal epsilonTerminal("");  return epsilonTerminal; }
		static const StringifiedNonTerminal &eof() { static StringifiedNonTerminal eofTerminal(eofString);  return eofTerminal; }

		StringifiedNonTerminal(const StringifiedNonTerminal &snt2);
		StringifiedNonTerminal(const std::string &_nonTerminalName);

		virtual StringifiedNonTerminal *duplicate_freeNeeded() const override;
	};

	// 产生式
	class Production
	{
		friend class Grammar;
	public:
		Grammar *grammarP;
		Index indexInGrammar;

		std::vector<SymbolIndex> leftIndices;
		std::vector<SymbolIndex> rightIndices;

		std::string getDescription() const;

	private:
		Production(Grammar *_grammarP, std::vector<SymbolIndex> _leftIndices, std::vector<SymbolIndex> _rightIndices, Index _indexInGrammar);
	};

	// LR(1) 项目
	class LRItem
	{
	public:
		Production *productionP;

		size_t dotPosition;
		SymbolIndex lookAheadIndex;

		std::string getDescription() const;
		
		bool isReducing() const;

		LRItem(Production &_production, size_t _dotPosition, SymbolIndex _lookAheadIndex);

		LRItem(const LRItem &lr2);

		bool operator==(const LRItem &lri) const;

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
		Grammar(std::istream &is);

		void setStartSymbol(const NonTerminal &nt);

		void addEOFSymbol();

		void addSymbol(const GrammarSymbol &_symbol);

		void addProduction(std::vector<std::string> _left, std::vector<std::string> _right);

		void generateFIRST();

		// 获取一序列文法符号的 FIRST
		std::set<SymbolIndex> getFIRST(std::vector<SymbolIndex> _letterIndices);

		template <class Out>
		void print(Out &out);

		~Grammar();
	};
}