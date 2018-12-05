#pragma once

namespace AkaneLang
{
	union TokenData
	{
		int i;
		unsigned ui;
		float f;
		double d;
	};


	struct TokenizedLetter : public Terminal
	{
		std::array<std::string, 2> type;
		std::vector<TokenData> data;
		std::vector<std::string> contents;

		TokenizedLetter();
		TokenizedLetter(const std::array<std::string, 2> &_type, const std::vector<TokenData> &_data, const std::vector<std::string> &_contents);

		static const TokenizedLetter &epsilon();
		static const TokenizedLetter &elseLetter();
		static const TokenizedLetter &eof();

		virtual std::string getShortDescription() const override;
		virtual std::string getLongDescription() const override;
		virtual std::string getUniqueName() const override;
		virtual TokenizedLetter *duplicate_freeNeeded() const override;
	};

	struct TokenizedLetterGenerator : public LetterGenerator
	{
		std::vector<TokenizedLetter> &tokLetters;
		Index i;
		bool isVectorInternal;

		virtual const TokenizedLetter *next_freeNeeded();
		virtual const TokenizedLetter *peek_freeNeeded();
		virtual void ignoreOne();

		TokenizedLetterGenerator();
		TokenizedLetterGenerator(const TokenizedLetterGenerator &_another);
		TokenizedLetterGenerator& operator=(const TokenizedLetterGenerator &_another);

		TokenizedLetterGenerator(std::vector<TokenizedLetter> &_tokLetters);
		~TokenizedLetterGenerator();
	};
}