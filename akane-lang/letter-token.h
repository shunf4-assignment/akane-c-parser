#pragma once
namespace AkaneLang
{
	struct LetterToken : public Terminal
	{
		Token token;

		LetterToken(const Token &);

		static const LetterToken &epsilon();
		static const LetterToken &elseLetter();
		static const LetterToken &eof();
		/*
		using Letter::operator<;
		using Letter::operator>;
		using Letter::operator==;
		using Letter::operator<=;
		using Letter::operator>=;
		using Letter::operator!=;
		*/
		virtual bool operator<(const Letter &r) const override;
		virtual bool operator>(const Letter &r) const override;
		virtual bool operator==(const Letter &r) const override;
		virtual bool operator<=(const Letter &r) const override;
		virtual bool operator>=(const Letter &r) const override;
		virtual bool operator!=(const Letter &r) const override;

		virtual std::string getUniqueName() const override { return token.uniqueStr(); }
		//virtual void setName(const std::string &_name) override { throw AkaneRuntimeException("不能对 LetterToken 调用 setName"); }
		virtual LetterToken *duplicate_freeNeeded() const override { return new LetterToken(token); };

		/*
		virtual bool operator<(const GrammarSymbol &r) const override;
		virtual bool operator>(const GrammarSymbol &r) const override;
		virtual bool operator==(const GrammarSymbol &r) const override;
		virtual bool operator<=(const GrammarSymbol &r) const override;
		virtual bool operator>=(const GrammarSymbol &r) const override;
		virtual bool operator!=(const GrammarSymbol &r) const override;
		*/

		virtual std::string getSymbolDescription() const override { return escape(token.uniqueStr()); }
		virtual std::string getLetterDescription() const override { return escape(token.uniqueStr()); }
		virtual std::string getLongDescription() const override { return token.getDescription(); }
	};


	struct LetterTokenGenerator : public LetterGenerator
	{
		static std::vector< LetterToken> staticV;
		std::vector< LetterToken> &v;
		decltype(v.cbegin()) p;
		virtual const LetterToken &next_freeNeeded();
		virtual const LetterToken &peek_freeNeeded();

		LetterTokenGenerator(const LetterTokenGenerator &_another) : v(_another.v)
		{
			p = v.cbegin();
		}

		LetterTokenGenerator(std::vector<AkaneLang::LetterToken> &_v) : v(_v)
		{
			p = v.cbegin();
		}
	};

}