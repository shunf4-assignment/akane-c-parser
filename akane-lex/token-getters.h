#pragma once

namespace AkaneLex
{
	class GetToken
	{
	public:
		static AkaneLang::Token getPreprocessorDirectiveToken(std::istringstream &s);
		static AkaneLang::Token getKeywordOrIDToken(std::istringstream &s);
		static AkaneLang::Token getIntegerToken(std::istringstream &s);
		static AkaneLang::Token getFloatToken(std::istringstream &s);
		static AkaneLang::Token getOperatorToken(std::istringstream &s);
		static AkaneLang::Token getDelimiterToken(std::istringstream &s);
		static AkaneLang::Token getNullToken(std::istringstream &s);
	};
}