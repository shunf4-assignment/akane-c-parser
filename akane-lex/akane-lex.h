#pragma once
#include "..\akane-lang\akane-lang.h"
#include <fstream>

DEFINE_AKANE_EXCEPTION(LexAnalyze)

namespace AkaneLex {
#include "internal-usings.h"
	struct Evaluator
	{
		// trim from start (in place)
		static void ltrim(std::string &s);

		// trim from end (in place)
		static void rtrim(std::string &s);

		// trim from both ends (in place)
		static void trim(std::string &s);

		virtual TokenizedLetter evaluate(std::istringstream &stream) const = 0;
	};

	struct NullEvaluator : Evaluator
	{
		virtual TokenizedLetter evaluate(std::istringstream &stream) const;
	};

	template <class AutomataState>
	struct LexDFA : public DFA<AutomataState>
	{
		const Evaluator *evaluatorP;

		//LexDFA(const std::vector<StringifiedLetter> &_alphabet, const std::map<StringifiedLetter, StringifiedLetter> &_tagifyMap, AkaneLang::LetterGenerator *_letterGeneratorP, Evaluator *_evaluatorP);
		LexDFA(AkaneLang::NFA<AutomataState> &nfa, const Evaluator *_evaluatorP);
		LexDFA(std::istream & is, StreamedLetterGenerator *stgP, const Evaluator *_evaluatorP);

		std::string typeStr() const override;
	};

	struct BaseLexicalAnalyzer
	{
		NullEvaluator ne;

		std::vector<LexDFA<BaseState>> dfas;

		std::vector<TokenizedLetter> output;

		StreamedLetterGenerator &inputStreamedLetterGenerator;
		TokenizedLetterGenerator outputTokenizedLetterGenerator;

		BaseLexicalAnalyzer(StreamedLetterGenerator &_inputStreamedLetterGenerator);

		virtual void analyze();
		virtual TokenizedLetterGenerator &getOutput();

		~BaseLexicalAnalyzer();
	};

/*
#ifdef CSTYLELEX_EXPORTS
#else
#define CSTYLELEX_API __declspec(dllimport)
	CSTYLELEX_API TokenizedLetterGenerator importedLexAnalyze(std::istream &is);
#endif
*/

}
