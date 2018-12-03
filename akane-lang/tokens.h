#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include "utils.h"

namespace AkaneLang
{
	enum TokenType
	{
		TNull = 0,
		TKeyword = 2,
		TIdentifier = 100,
		TInteger = 101,
		TFloat = 102,
		TOperator = 300,
		TDelimiter = 400,
		TPreprocessDirective = 500,
		TInvalid = (std::numeric_limits<unsigned short>::max)(),
		TElse = (std::numeric_limits<unsigned short>::max)() - 1,
		TEOF = (std::numeric_limits<unsigned short>::max)() - 2,
	};

	typedef unsigned TokenSubType;
	typedef unsigned TokenStrictType;

#ifndef AKANELANG_DO_NOT_REDEFINE_TOKENTYPE
#define TokenType TokenType
#endif

	union TokenData
	{
		int i;
		unsigned ui;
		float f;
		double d;
	};

	struct Token
	{
		TokenType type;
		TokenSubType subType;

		// Token 的信息存在 data 和/或 texts 中
		std::vector<TokenData> data;
		std::vector<std::string> texts;

		std::string getDescription() const;
		TokenStrictType getStrictType() const;
		bool typeEqualTo(const Token &t2) { return type == t2.type; }
		bool strictTypeEqualTo(const Token &t2) { return type == t2.type && subType == t2.subType; }
		//bool fullyEqualTo(const Token &t2) { return type == t2.type && subType == t2.subType && data == t2.data && texts == t2.texts; }

		bool operator==(const Token &t2) { return strictTypeEqualTo(t2); }
		operator bool() { return type != TokenType::TNull && type != TokenType::TInvalid; }

		Token() : type(TokenType::TNull), subType(0) {}
		Token(TokenType _type, TokenSubType _subType) : type(_type), subType(_subType) {}

		std::string typeToDispStr() const;
		std::string typeToUniqueStr() const;
		std::string subTypeToDispStr() const;
		std::string subTypeToUniqueStr() const;
		std::string uniqueStr() const;

	};
}