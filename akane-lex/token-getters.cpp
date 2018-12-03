#include "stdafx.h"
#include "akane-lex.h"
#include <iostream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include "token-getters.h"

using namespace std;
using namespace AkaneLang;

namespace AkaneLex
{
	// trim from start (in place)
	static inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place)
	static inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

	const set<string> keywordSet =
	{
		"auto",
		"break",
		"case",
		"char",
		"const",
		"continue",
		"default",
		"do",
		"double",
		"else",
		"enum",
		"extern",
		"float",
		"for",
		"goto",
		"if",
		"inline",
		"int",
		"long",
		"register",
		"restrict",
		"return",
		"short",
		"signed",
		"sizeof",
		"static",
		"struct",
		"switch",
		"typedef",
		"union",
		"unsigned",
		"void",
		"volatile",
		"while",
		"_Alignas",
		"_Alignof",
		"_Atomic",
		"_Bool",
		"_Complex",
		"_Generic",
		"_Imaginary",
		"_Noreturn",
		"_Static_assert",
		"_Thread_local"
	};

	const set<string> operatorSet =
	{
		"+", "-", "*", "/", "=", "==", ">", ">=", "<", "<=", "!="
	};

	const set<string> delimiterSet =
	{
		"\"", "[", "]", "(", ")", "\'", "{", "}", ";"
	};

	Token GetToken::getPreprocessorDirectiveToken(istringstream &s)
	{
		string str = s.str();
		trim(str);

		if (str.length() <= 1 || str[0] != '#')
		{
			throw AkaneLexAnalyzeException("预处理器指令错误: %s", str.c_str());
		}

		str.erase(0, 1);

		Token t;
		t.type = TokenType::TPreprocessDirective;
		t.subType = 0;
		t.texts.push_back(str);

		return t;
	}

	Token GetToken::getKeywordOrIDToken(istringstream &s)
	{
		string str = s.str();

		if (str.length() == 0)
		{
			throw AkaneLexAnalyzeException("标识符长度为 0");
		}

		Token t;
		if (keywordSet.count(str) == 1)
		{
			t.type = TokenType::TKeyword;
			t.subType = distance(keywordSet.begin(), keywordSet.find(str));
			t.texts.push_back(str);
		}
		else
		{
			t.type = TokenType::TIdentifier;
			t.subType = 0;
			t.texts.push_back(str);
		}

		return t;
	}

	Token GetToken::getIntegerToken(istringstream &s)
	{
		string str = s.str();

		if (str.length() == 0)
		{
			throw AkaneLexAnalyzeException("integer 长度为 0");
		}

		Token t;
		t.type = TokenType::TInteger;
		t.subType = 0;
		t.data.push_back(TokenData());
		t.texts.push_back(str);

		s >> t.data[0].ui;
		if (s.fail())
		{
			s.clear();
			throw AkaneLexAnalyzeException("integer 字符串不好: %s", str.c_str());
		}

		return t;
	}

	Token GetToken::getFloatToken(istringstream &s)
	{
		string str = s.str();

		if (str.length() == 0)
		{
			throw AkaneLexAnalyzeException("float 长度为 0");
		}

		Token t;
		t.type = TokenType::TFloat;
		t.subType = 0;
		t.data.push_back(TokenData());
		t.texts.push_back(str);

		s >> t.data[0].d;
		if (s.fail())
		{
			s.clear();
			throw AkaneLexAnalyzeException("float 字符串不好: %s", str.c_str());
		}

		return t;
	}

	Token GetToken::getOperatorToken(istringstream &s)
	{
		string str = s.str();
		auto findResult = operatorSet.find(str);
		if (findResult == operatorSet.end())
		{
			throw AkaneLexAnalyzeException("operator 字符串不好: %s", str.c_str());
		}

		Token t;
		t.type = TokenType::TOperator;
		t.subType = distance(operatorSet.begin(), findResult);
		t.texts.push_back(str);

		return t;
	}

	Token GetToken::getDelimiterToken(istringstream &s)
	{
		string str = s.str();
		auto findResult = delimiterSet.find(str);
		if (findResult == delimiterSet.end())
		{
			throw AkaneLexAnalyzeException("delimiter 字符串不好: %s", str.c_str());
		}

		Token t;
		t.type = TokenType::TDelimiter;
		t.subType = distance(delimiterSet.begin(), findResult);
		t.texts.push_back(str);

		return t;
	}

	Token GetToken::getNullToken(istringstream &s)
	{
		Token t;
		t.type = TokenType::TNull;
		t.subType = 0;

		return t;
	}
}