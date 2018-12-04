#include "stdafx.h"
#include "akane-lang.h"
using namespace AkaneLang;

AkaneLang::TokenizedLetter::TokenizedLetter() 
	: type({ "[Undefined]", "" }), data({}), contents({})
{
}

AkaneLang::TokenizedLetter::TokenizedLetter(const std::array<std::string, 2>& _type, const std::vector<TokenData>& _data, const std::vector<std::string>& _contents) : type(_type), data(_data), contents(_contents)
{
}

const TokenizedLetter & AkaneLang::TokenizedLetter::epsilon()
{
	static TokenizedLetter e = TokenizedLetter({ "[Epsilon]", "" }, {}, {});
	return e;
}

const TokenizedLetter & AkaneLang::TokenizedLetter::elseLetter()
{
	static TokenizedLetter e = TokenizedLetter({ "[Else]", "" }, {}, {});
	return e;
}

const TokenizedLetter & AkaneLang::TokenizedLetter::eof()
{
	static TokenizedLetter e = TokenizedLetter({ "[EOF]", "" }, {}, {});
	return e;
}

std::string AkaneLang::TokenizedLetter::getShortDescription() const
{
	return type[0] + (type[1].size() ? (std::string("-") + type[1]) : std::string());
}

std::string AkaneLang::TokenizedLetter::getLongDescription() const
{
	return std::string("<") + type[0] + (type[1].length() ? (", " + type[1]) : std::string()) + (contents.size() ? (std::string(", \"") + contents[0] + "\"") : std::string()) + ">";
}

std::string AkaneLang::TokenizedLetter::getUniqueName() const
{
	return type[1].length() ? type[1] : type[0];
}

TokenizedLetter * AkaneLang::TokenizedLetter::duplicate_freeNeeded() const
{
	return new TokenizedLetter(*this);
}

// 记得释放
#pragma warning(push)
#pragma warning(disable:4172)
const TokenizedLetter *AkaneLang::TokenizedLetterGenerator::next_freeNeeded()
{
	if (i == tokLetters.size())
	{
		throw AkaneGetEOFException("无法再读入");
	}

	auto q = new TokenizedLetter(this->tokLetters[i++]);
	return q;
}

const TokenizedLetter *AkaneLang::TokenizedLetterGenerator::peek_freeNeeded()
{
	if (i == tokLetters.size())
	{
		//throw AkaneGetEOFException("无法再读入");
		return new TokenizedLetter(TokenizedLetter::eof());
	}

	auto q = new TokenizedLetter(tokLetters[i]);
	return q;
}
#pragma warning(pop)

void AkaneLang::TokenizedLetterGenerator::ignoreOne()
{
	if (i == tokLetters.size())
	{
		throw AkaneGetEOFException("无法再读入");
	}

	i++;
}

AkaneLang::TokenizedLetterGenerator::TokenizedLetterGenerator(const TokenizedLetterGenerator & _another) : tokLetters(_another.tokLetters), i(0)
{
}

AkaneLang::TokenizedLetterGenerator::TokenizedLetterGenerator(std::vector<TokenizedLetter>& _tokLetters) : tokLetters(_tokLetters), i(0)
{
}
