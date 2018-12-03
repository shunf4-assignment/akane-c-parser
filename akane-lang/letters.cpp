#include "stdafx.h"
#include "akane-lang.h"
using namespace AkaneLang;

AkaneLang::LetterString::LetterString(const std::string &_letter) : text(_letter)
{
}

const LetterString & AkaneLang::LetterString::epsilon()
{
	static LetterString e("");
	return e;
}

const LetterString & AkaneLang::LetterString::elseLetter()
{
	static LetterString e("[else]");
	return e;
}

bool AkaneLang::LetterString::operator<(const Letter & r) const
{
	return text < dynamic_cast<const LetterString &>(r).text;
}

bool AkaneLang::LetterString::operator>(const Letter & r)const
{
	return text > dynamic_cast<const LetterString &>(r).text;
}

bool AkaneLang::LetterString::operator==(const Letter & r) const
{
	return text == dynamic_cast<const LetterString &>(r).text;
}

bool AkaneLang::LetterString::operator<=(const Letter & r) const
{
	return text <= dynamic_cast<const LetterString &>(r).text;
}

bool AkaneLang::LetterString::operator>=(const Letter & r) const
{
	return text >= dynamic_cast<const LetterString &>(r).text;
}

bool AkaneLang::LetterString::operator!=(const Letter & r) const
{
	return text != dynamic_cast<const LetterString &>(r).text;
}

// �ǵ��Լ��ͷ�
const LetterString &AkaneLang::LetterStringGenerator::next_freeNeeded()
{
	char c;
	c = s.get();
	if (!s.good() || c == EOF)
	{
		throw AkaneGetEOFException("�޷��ٶ���");
	}
	currWordStream << c;
	std::string str(1, c);

	auto p = new LetterString(str);
	return *p;
}

// �ǵ��Լ��ͷ�
const LetterString &AkaneLang::LetterStringGenerator::peek_freeNeeded()
{
	char c;
	c = s.peek();
	if (!s.good() || c == EOF)
	{
		throw AkaneGetEOFException("�޷��ٶ���");
	}
	std::string str(1, c);

	auto p = new LetterString(str);
	return *p;
}

AkaneLang::LetterStringGenerator::LetterStringGenerator(std::istream & _s) : s(_s), currWordStream()
{
	lastPos = s.tellg();
}


AkaneLang::LetterToken::LetterToken(const AkaneLang::Token &t)
{
	token = t;
}

const LetterToken & AkaneLang::LetterToken::epsilon()
{
	static AkaneLang::Token t;
	static LetterToken e(t);
	return e;
}

const LetterToken & AkaneLang::LetterToken::elseLetter()
{
	static AkaneLang::Token t(TokenType::TElse, 0);
	static LetterToken e(t);
	return e;
}

const LetterToken & AkaneLang::LetterToken::eof()
{
	static AkaneLang::Token t(TokenType::TEOF, 0);
	static LetterToken e(t);
	return e;
}

bool AkaneLang::LetterToken::operator<(const Letter & r) const
{
	try {
		return token.getStrictType() < dynamic_cast<const LetterToken &>(r).token.getStrictType();
	}
	catch (std::bad_cast)
	{
		return AkaneLang::GrammarSymbol::operator<(r);
	}
}

bool AkaneLang::LetterToken::operator>(const Letter & r) const
{
	try {
		return token.getStrictType() > dynamic_cast<const LetterToken &>(r).token.getStrictType();
	}
	catch (std::bad_cast)
	{
		return AkaneLang::GrammarSymbol::operator>(r);
	}
}

bool AkaneLang::LetterToken::operator==(const Letter & r) const
{
	try {
		return token.getStrictType() == dynamic_cast<const LetterToken &>(r).token.getStrictType();
	}
	catch (std::bad_cast)
	{
		return AkaneLang::GrammarSymbol::operator==(r);
	}
}

bool AkaneLang::LetterToken::operator<=(const Letter & r) const
{
	try {
		return token.getStrictType() <= dynamic_cast<const LetterToken &>(r).token.getStrictType();
	}
	catch (std::bad_cast)
	{
		return AkaneLang::GrammarSymbol::operator<=(r);
	}
}

bool AkaneLang::LetterToken::operator>=(const Letter & r) const
{
	try {
		return token.getStrictType() >= dynamic_cast<const LetterToken &>(r).token.getStrictType();
	}
	catch (std::bad_cast)
	{
		return AkaneLang::GrammarSymbol::operator>=(r);
	}
}

bool AkaneLang::LetterToken::operator!=(const Letter & r) const
{
	try {
		return token.getStrictType() != dynamic_cast<const LetterToken &>(r).token.getStrictType();
	}
	catch (std::bad_cast)
	{
		return AkaneLang::GrammarSymbol::operator!=(r);
	}
}

// �ǵ��ͷ�
#pragma warning(push)
#pragma warning(disable:4172)
const LetterToken & AkaneLang::LetterTokenGenerator::next_freeNeeded()
{
	if (p == v.cend())
	{
		throw AkaneGetEOFException("�޷��ٶ���");
	}

	auto q = new LetterToken(*p++);
	return *q;
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4172)
const LetterToken & AkaneLang::LetterTokenGenerator::peek_freeNeeded()
{
	if (p == v.cend())
	{
		//throw AkaneGetEOFException("�޷��ٶ���");
		auto q = new LetterToken(LetterToken::eof());
		return *q;
	}
	auto q = new LetterToken(*p);
	return *q;
}
#pragma warning(pop)
