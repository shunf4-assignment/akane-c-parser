#pragma once

#include "akane-lex.h"

namespace AkaneLex {
	namespace CStyle
	{
#include "internal-usings.h"

		struct PreprocessorDirectiveEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct KeywordOrIDEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct IntegerEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct FloatEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct OperatorEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct DelimiterEvaluator : Evaluator
		{
			TokenizedLetter evaluate(std::istringstream &stream) const override;
		};

		struct CStyleLexicalAnalyzer : BaseLexicalAnalyzer
		{
			PreprocessorDirectiveEvaluator ppde;
			KeywordOrIDEvaluator kide;
			IntegerEvaluator ie;
			FloatEvaluator fe;
			OperatorEvaluator oe;
			DelimiterEvaluator de;

			CStyleLexicalAnalyzer(StreamedLetterGenerator &_inputStreamedLetterGenerator);

			virtual void analyze();

			~CStyleLexicalAnalyzer();
		};
	}
}