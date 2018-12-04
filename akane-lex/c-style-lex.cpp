#include "stdafx.h"
#include "c-style-lex.h"

using namespace AkaneLang;
using namespace AkaneLex;
using namespace std;

static const set<string> keywordSet =
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

static const set<string> operatorSet =
{
	"+", "-", "*", "/", "=", "==", ">", ">=", "<", "<=", "!="
};

static const set<string> delimiterSet =
{
	"\"", "[", "]", "(", ")", "\'", "{", "}", ";"
};

TokenizedLetter AkaneLex::CStyle::PreprocessorDirectiveEvaluator::evaluate(std::istringstream & stream) const
{
	string str = stream.str();
	Evaluator::trim(str);

	if (str.length() <= 1 || str[0] != '#')
	{
		throw AkaneLexAnalyzeException("预处理器指令错误: %s", str.c_str());
	}

	str.erase(0, 1);

	TokenizedLetter t;
	t.type = {"预处理器指令", ""};
	t.contents.push_back(str);

	return t;
}

TokenizedLetter AkaneLex::CStyle::KeywordOrIDEvaluator::evaluate(std::istringstream & stream) const
{
	string str = stream.str();

	if (str.length() == 0)
	{
		throw AkaneLexAnalyzeException("标识符长度为 0");
	}

	TokenizedLetter t;
	if (keywordSet.count(str) == 1)
	{
		t.type = { "关键字", str };
		t.contents.push_back(str);
	}
	else
	{
		t.type = { "标识符", "" };
		t.contents.push_back(str);
	}

	return t;
}

TokenizedLetter AkaneLex::CStyle::IntegerEvaluator::evaluate(std::istringstream & stream) const
{
	string str = stream.str();

	if (str.length() == 0)
	{
		throw AkaneLexAnalyzeException("integer 长度为 0");
	}

	TokenizedLetter t;
	t.type = { "整数", "" };
	t.data.push_back(TokenData());
	t.contents.push_back(str);

	stream >> t.data[0].ui;
	if (stream.fail())
	{
		stream.clear();
		throw AkaneLexAnalyzeException("integer 字符串不好: %s", str.c_str());
	}

	return t;
}

TokenizedLetter AkaneLex::CStyle::FloatEvaluator::evaluate(std::istringstream & stream) const
{
	string str = stream.str();

	if (str.length() == 0)
	{
		throw AkaneLexAnalyzeException("float 长度为 0");
	}

	TokenizedLetter t;
	t.type = {"浮点数", ""};
	t.data.push_back(TokenData());
	t.contents.push_back(str);

	stream >> t.data[0].d;
	if (stream.fail())
	{
		stream.clear();
		throw AkaneLexAnalyzeException("float 字符串不好: %s", str.c_str());
	}

	return t;
}

TokenizedLetter AkaneLex::CStyle::OperatorEvaluator::evaluate(std::istringstream & stream) const
{
	string str = stream.str();
	auto findResult = operatorSet.find(str);
	if (findResult == operatorSet.end())
	{
		throw AkaneLexAnalyzeException("operator 字符串不好: %s", str.c_str());
	}

	TokenizedLetter t;
	t.type = {"运算符", str};
	t.contents.push_back(str);

	return t;
}

TokenizedLetter AkaneLex::CStyle::DelimiterEvaluator::evaluate(std::istringstream & stream) const
{
	string str = stream.str();
	auto findResult = delimiterSet.find(str);
	if (findResult == delimiterSet.end())
	{
		throw AkaneLexAnalyzeException("delimiter 字符串不好: %s", str.c_str());
	}

	TokenizedLetter t;
	t.type = {"界符", str};
	t.contents.push_back(str);

	return t;
}

AkaneLex::CStyle::CStyleLexicalAnalyzer::CStyleLexicalAnalyzer(StreamedLetterGenerator &_inputStreamedLetterGenerator) : BaseLexicalAnalyzer(_inputStreamedLetterGenerator)
{
	ifstream fppd("preprocessor-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(fppd, &inputStreamedLetterGenerator, &ppde));

	ifstream fws("whitespaces-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(fws, &inputStreamedLetterGenerator, &ne));

	ifstream fkid("keywordAndIdentifier-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(fkid, &inputStreamedLetterGenerator, &kide));

	ifstream fi("integer-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(fi, &inputStreamedLetterGenerator, &ie));

	ifstream ff("float-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(ff, &inputStreamedLetterGenerator, &fe));

	ifstream fc("comment-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(fc, &inputStreamedLetterGenerator, &ne));

	ifstream fd("delimiter-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(fd, &inputStreamedLetterGenerator, &de));

	ifstream fo("operator-nfa.txt", ios::in);
	dfas.push_back(LexDFA<BaseState>(fo, &inputStreamedLetterGenerator, &oe));
}

void AkaneLex::CStyle::CStyleLexicalAnalyzer::analyze()
{
	BaseLexicalAnalyzer::analyze();
}

AkaneLex::CStyle::CStyleLexicalAnalyzer::~CStyleLexicalAnalyzer()
{
}

