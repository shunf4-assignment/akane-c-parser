#ifdef AKANELEX_EXPORTS
#define AKANELEX_API __declspec(dllexport)
#else
#define AKANELEX_API __declspec(dllimport)
#endif

#include "..\akane-lang\akane-lang.h"
#include <fstream>

DEFINE_AKANE_EXCEPTION(LexAnalyze)

namespace AkaneLex {

	enum TokenType
	{
		TInvalid = 0,
		TKeyword = 1,
		TIdentifier = 100,
		TInteger = 101,
		TFloat = 102,
		TOperator = 300,
		TDelimiter = 400,
		TPreprocessInstruction = 500
	};

#ifndef AKANELEX_DO_NOT_REDEFINE_TOKENTYPE
#define TokenType AkaneLex::TokenType
#endif

	union TokenData
	{
		int i;
		float f;
		double d;
	};

	struct Token
	{
		std::string getDescription();
		TokenType type;
		TokenData data;
		std::string text;
	};

	class LexicalAnalyzer
	{
	public:
		std::vector< AkaneLang::DFA<AkaneLang::LetterString, AkaneLang::SimpleState> *> dfaPointers;
		std::istream *streamPointer;
		AkaneLang::LetterStringGenerator lsg;
		std::vector< Token> output;

		LexicalAnalyzer(std::istream *streamPointer);
		AkaneLang::DFA<AkaneLang::LetterString, AkaneLang::SimpleState> &makeDFA(std::istream &is, const std::string &dfaName);
		void analyze();
		std::vector< Token> getOutput();

		
		~LexicalAnalyzer();
	};
}

namespace AkaneLang
{
	struct LetterToken : virtual AkaneLang::Letter
	{
		AkaneLex::Token token;

		LetterToken(const AkaneLex::Token &);

		static const LetterToken &epsilon();
		virtual bool operator<(const Letter &r) const;
		virtual bool operator>(const Letter &r) const;
		virtual bool operator==(const Letter &r) const;
		virtual bool operator<=(const Letter &r) const;
		virtual bool operator>=(const Letter &r) const;
		virtual bool operator!=(const Letter &r) const;

		virtual std::string getValue() const { return token.text; }
		virtual std::string getDescription() const { return token.text; }
	};

	struct LetterTokenGenerator : public virtual LetterGenerator
	{
		std::vector< AkaneLex::Token> v;
		decltype(v.cbegin()) p;
		virtual const LetterToken &next_freeNeeded();
		virtual const LetterToken &peek_freeNeeded();
		LetterTokenGenerator(LetterTokenGenerator &_another) : v(_another.v)
		{
			p = v.cbegin();
		}

		LetterTokenGenerator(const std::vector< AkaneLex::Token> &_v) : v(_v)
		{
			p = v.cbegin();
		}
	};
}