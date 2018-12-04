#include "stdafx.h"
#include "akane-lang.h"
using namespace AkaneLang;
using namespace std;

string AkaneLang::unescape(const string & orig)
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

StringifiedLetter AkaneLang::unescape(const StringifiedLetter & l)
{
	return StringifiedLetter(unescape(l.name));
}

string AkaneLang::escape(const string & orig)
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
	if (orig == eofString)
	{
		return ("[EOF]");
	}

	return orig;
}
