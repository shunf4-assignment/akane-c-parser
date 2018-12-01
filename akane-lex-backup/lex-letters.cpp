#include "stdafx.h"
#include "akane-lex.h"
using namespace AkaneLex;
using namespace AkaneLang;

AkaneLang::LetterToken::LetterToken(const AkaneLex::Token &)
{
}

const LetterToken & AkaneLang::LetterToken::epsilon()
{
	static AkaneLex::Token t;
	static LetterToken e(t);
	return e;
}

bool AkaneLang::LetterToken::operator<(const Letter & r) const
{
	return token.type < dynamic_cast<const LetterToken &>(r).token.type;
}

bool AkaneLang::LetterToken::operator>(const Letter & r) const
{
	return token.type > dynamic_cast<const LetterToken &>(r).token.type;
}

bool AkaneLang::LetterToken::operator==(const Letter & r) const
{
	return token.type == dynamic_cast<const LetterToken &>(r).token.type;
}

bool AkaneLang::LetterToken::operator<=(const Letter & r) const
{
	return token.type <= dynamic_cast<const LetterToken &>(r).token.type;
}

bool AkaneLang::LetterToken::operator>=(const Letter & r) const
{
	return token.type >= dynamic_cast<const LetterToken &>(r).token.type;
}

bool AkaneLang::LetterToken::operator!=(const Letter & r) const
{
	return token.type != dynamic_cast<const LetterToken &>(r).token.type;
}

// 记得释放
const LetterToken & AkaneLang::LetterTokenGenerator::next_freeNeeded()
{
	if (p == v.cend())
	{
		throw AkaneEOFException("无法再读入");
	}
	auto q = new Token(*p++);
	return *q;
}

const LetterToken & AkaneLang::LetterTokenGenerator::peek_freeNeeded()
{
	if (p == v.cend())
	{
		throw AkaneEOFException("无法再读入");
	}
	auto q = new Token(*p);
	return *q;
}