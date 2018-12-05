#include "stdafx.h"
#include "akane-lang.h"
using namespace AkaneLang;

AkaneLang::StringifiedLetter::StringifiedLetter()
	: Letter(), name("(invalid)")
{
}

AkaneLang::StringifiedLetter::StringifiedLetter(const StringifiedLetter & _l)
	: Letter(_l), name(_l.name)
{
}

AkaneLang::StringifiedLetter::StringifiedLetter(const std::string &_name) : name(_name)
{
}

AkaneLang::StringifiedLetter::StringifiedLetter(const TokenizedLetter & lt) : name(lt.getUniqueName())
{
	if (lt == TokenizedLetter::epsilon())
		name = StringifiedLetter::epsilon().name;

	if (lt == TokenizedLetter::eof())
		name = StringifiedLetter::eof().name;

	if (lt == TokenizedLetter::elseLetter())
		name = StringifiedLetter::elseLetter().name;
}

StringifiedLetter & AkaneLang::StringifiedLetter::operator=(const StringifiedLetter & right)
{
	name = right.name;
	return *this;
}

const StringifiedLetter & AkaneLang::StringifiedLetter::epsilon()
{
	static StringifiedLetter e("");
	return e;
}

const StringifiedLetter & AkaneLang::StringifiedLetter::elseLetter()
{
	static StringifiedLetter e("[else]");
	return e;
}

const StringifiedLetter & AkaneLang::StringifiedLetter::eof()
{
	static StringifiedLetter e(eofString);
	return e;
}

std::string AkaneLang::StringifiedLetter::getUniqueName() const
{
	return name;
}

std::string AkaneLang::StringifiedLetter::getShortDescription() const
{
	return escape(name);
}

std::string AkaneLang::StringifiedLetter::getLongDescription() const
{
	return std::string("[ ") + escape(name) + " ]";
}

StringifiedLetter * AkaneLang::StringifiedLetter::duplicate_freeNeeded() const
{
	return new StringifiedLetter(*this);
}


// 记得自己释放
const StringifiedLetter *AkaneLang::StreamedLetterGenerator::next_freeNeeded()
{
	char c;
	c = streamP->get();
	if (!streamP->good() || c == EOF)
	{
		throw AkaneGetEOFException("无法再读入");
	}
	currLexemeStream << c;
	std::string str(1, c);

	auto p = new StringifiedLetter(str);
	return p;
}

// 记得自己释放
const StringifiedLetter *AkaneLang::StreamedLetterGenerator::peek_freeNeeded()
{
	char c;
	c = streamP->peek();
	if (!streamP->good() || c == EOF)
	{
		throw AkaneGetEOFException("无法再读入");
	}
	std::string str(1, c);

	auto p = new StringifiedLetter(str);
	return p;
}

void AkaneLang::StreamedLetterGenerator::ignoreOne()
{
	char c;
	c = streamP->get();
	if (!streamP->good() || c == EOF)
	{
		throw AkaneGetEOFException("无法再读入");
	}
	currLexemeStream << c;
}

AkaneLang::StreamedLetterGenerator::StreamedLetterGenerator(std::istream & _s) : streamP(&_s), currLexemeStream(), lastPos(streamP->tellg())
{
}

AkaneLang::Letter::~Letter()
{
}
