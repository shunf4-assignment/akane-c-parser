#include "akane-lang.h"
using namespace AkaneLang;

const Letter &AkaneLang::Letter::epsilon()
{
	return *(static_cast<const Letter *>(nullptr));
}

AkaneLang::LetterString::LetterString(const std::string &_letter) : text(_letter)
{
}

const LetterString & AkaneLang::LetterString::epsilon()
{
	static LetterString e("");
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

std::string AkaneLang::LetterString::unescape(const std::string & orig)
{
	if (orig == "[space]")
	{
		return " ";
	}
	if (orig == "[newline]")
	{
		return ("\n");
	}
	if (orig == "[tab]")
	{
		return ("\t");
	}
	if (orig == "[vtab]")
	{
		return ("\v");
	}
	return orig;
}

LetterString AkaneLang::LetterString::unescape(const LetterString & l)
{
	return LetterString(unescape(l.getValue()));
}

std::string AkaneLang::LetterString::escape(const std::string & orig)
{
	if (orig == " ")
	{
		return "[space]";
	}
	if (orig == "\n")
	{
		return ("[newline]");
	}
	if (orig == "\t")
	{
		return ("[tab]");
	}
	if (orig == "\v")
	{
		return ("[vtab]");
	}
	if (orig == "")
	{
		return ("[epsilon]");
	}

	return orig;
}

// 记得自己释放
const LetterString &AkaneLang::LetterStringGenerator::next_freeNeeded()
{
	char c;
	c = s.get();
	if (!s.good() || c == EOF)
	{
		throw AkaneGetEOFException("无法再读入");
	}
	currWord << c;
	std::string str(1, c);

	auto p = new LetterString(str);
	return *p;
}

// 记得自己释放
const LetterString &AkaneLang::LetterStringGenerator::peek_freeNeeded()
{
	char c;
	c = s.peek();
	if (!s.good() || c == EOF)
	{
		throw AkaneGetEOFException("无法再读入");
	}
	std::string str(1, c);
	
	auto p = new LetterString(str);
	return *p;
}

AkaneLang::LetterStringGenerator::LetterStringGenerator(std::istream & _s) : s(_s), currWord()
{
	lastPos = s.tellg();
}
