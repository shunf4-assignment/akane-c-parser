
#include "pch.h"
#include <iostream>
#include "..\akane-lex\akane-lex.h"
#include "..\akane-lang\akane-lang.h"
using namespace std;


int main()
{
	ifstream f("test-text.txt", ios::in);
	AkaneLex::LexicalAnalyzer lex(&f);
	lex.analyze();
}
