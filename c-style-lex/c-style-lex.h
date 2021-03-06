#ifdef CSTYLELEX_EXPORTS
#define CSTYLELEX_API __declspec(dllexport)
#else
#define CSTYLELEX_API __declspec(dllimport)
#endif

#include "akane-lex.h"

namespace AkaneLex {
#include "internal-usings.h"

	namespace CStyle
	{
		struct CSTYLELEX_API PreprocessorDirectiveEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct CSTYLELEX_API KeywordOrIDEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct CSTYLELEX_API IntegerEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct CSTYLELEX_API FloatEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct CSTYLELEX_API OperatorEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct DelimiterEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};
	}
	struct CSTYLELEX_API CStyleLexicalAnalyzer : BaseLexicalAnalyzer
	{
		CStyle::PreprocessorDirectiveEvaluator ppde;
		CStyle::KeywordOrIDEvaluator kide;
		CStyle::IntegerEvaluator ie;
		CStyle::FloatEvaluator fe;
		CStyle::OperatorEvaluator oe;
		CStyle::DelimiterEvaluator de;

		CStyleLexicalAnalyzer(StreamedLetterGenerator &_inputStreamedLetterGenerator);

		void analyze();

		~CStyleLexicalAnalyzer();
	};
}

CSTYLELEX_API void importedLexAnalyze(AkaneLang::TokenizedLetterGenerator &result, AkaneLang::StreamedLetterGenerator &slg);