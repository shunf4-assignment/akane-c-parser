#pragma once
#define AKANE_EXCEPTION_PRINT
#include "utils.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <array>

#define GET_ASSIGN_AND_FREE(type, result, thing) \
	const type & result ## R = dynamic_cast<const type &>(thing);\
	type result = result ## R;\
	delete &result##R;

DEFINE_AKANE_EXCEPTION(GetEOF)
DEFINE_AKANE_EXCEPTION(Lang)
DEFINE_AKANE_EXCEPTION(InputValue)
DEFINE_AKANE_EXCEPTION(Runtime)


namespace AkaneLang
{
	const std::string eofString{ '\0' };

	struct StringifiedLetter;
	std::string unescape(const std::string &str);
	StringifiedLetter unescape(const StringifiedLetter &l);
	std::string escape(const std::string &orig);
}

#pragma warning(disable: 4250)
//#include "tokens.h"

#include "letters.h"

#include "grammar.h"

#include "tokenized-letter.h"

#include "automata-states.h"

#include "automatas.h"

