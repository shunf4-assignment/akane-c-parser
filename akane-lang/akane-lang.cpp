#include "stdafx.h"
#include "akane-lang.h"
using namespace AkaneLang;

std::string AkaneLang::unescape(const std::string & orig)
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

LetterString AkaneLang::unescape(const LetterString & l)
{
	return LetterString(unescape(l.text));
}

std::string AkaneLang::escape(const std::string & orig)
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
	if (orig == zero)
	{
		return ("[EOF]");
	}

	return orig;
}
