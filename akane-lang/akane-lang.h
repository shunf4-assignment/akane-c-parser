#pragma once
#define AKANE_EXCEPTION_PRINT
#include "utils.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>

#define GET_ASSIGN_AND_FREE(type, result, thing) \
	type & result ## R = dynamic_cast< type &>(thing);\
	type result = result ## R;\
	delete &result##R;

DEFINE_AKANE_EXCEPTION(GetEOF)
DEFINE_AKANE_EXCEPTION(Lang)
DEFINE_AKANE_EXCEPTION(InputValue)

namespace AkaneLang
{
	const std::string zero{ '\0' };

	struct Letter;
	struct LetterString;

	std::string unescape(const std::string &str);
	LetterString unescape(const LetterString &l);
	std::string escape(const std::string &orig);
}

#pragma warning(disable: 4250)
#include "tokens.h"

#include "letters.h"

#include "grammarsymbols.h"

#include "lettertoken.h"

#include "automata-states.h"

#include "automatas.h"

