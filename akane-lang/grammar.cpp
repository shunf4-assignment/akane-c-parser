#include "stdafx.h"
#include "akane-lang.h"

using namespace AkaneLang;
using namespace std;

/* ==== 文法符号元素 ==== */

AkaneLang::GrammarSymbol::~GrammarSymbol()
{
}

const StringifiedGrammarSymbol & AkaneLang::StringifiedGrammarSymbol::epsilon()
{
	static StringifiedGrammarSymbol staticSGS = StringifiedGrammarSymbol("");
	return staticSGS;
}

AkaneLang::StringifiedGrammarSymbol::StringifiedGrammarSymbol() : StringifiedLetter()
{
}

AkaneLang::StringifiedGrammarSymbol::StringifiedGrammarSymbol(const StringifiedGrammarSymbol & s2) : StringifiedLetter(s2)
{
}

AkaneLang::StringifiedGrammarSymbol::StringifiedGrammarSymbol(const std::string & _name) : StringifiedLetter(_name)
{
}

AkaneLang::StringifiedGrammarSymbol::StringifiedGrammarSymbol(const TokenizedLetter & lt) : StringifiedLetter(lt)
{
}

bool AkaneLang::StringifiedGrammarSymbol::isTerminal() const
{ 
	throw AkaneNotImplementedException("");
}
/*
StringifiedGrammarSymbol *AkaneLang::StringifiedGrammarSymbol::duplicate_freeNeeded() const
{
	return new StringifiedGrammarSymbol(*this);
}
*/
AkaneLang::StringifiedGrammarSymbol::~StringifiedGrammarSymbol()
{
}

bool AkaneLang::Terminal::isTerminal() const
{
	return true;
}

AkaneLang::Terminal::~Terminal()
{
}

bool AkaneLang::StringifiedTerminal::isTerminal() const
{
	return Terminal::isTerminal();
}

const StringifiedTerminal & AkaneLang::StringifiedTerminal::epsilon()
{
	static StringifiedTerminal epsilonTerminal(""); 
	return epsilonTerminal;
}

const StringifiedTerminal & AkaneLang::StringifiedTerminal::eof()
{
	static StringifiedTerminal eofTerminal(eofString); 
	return eofTerminal;
}

AkaneLang::StringifiedTerminal::StringifiedTerminal(const StringifiedTerminal & st2) : StringifiedGrammarSymbol(st2)
{
}

AkaneLang::StringifiedTerminal::StringifiedTerminal(const std::string & _terminalName) : StringifiedGrammarSymbol(_terminalName)
{
}

StringifiedTerminal * AkaneLang::StringifiedTerminal::duplicate_freeNeeded() const
{
	return new StringifiedTerminal(*this);
}

bool AkaneLang::StringifiedNonTerminal::isTerminal() const
{
	return NonTerminal::isTerminal();
}

AkaneLang::StringifiedNonTerminal::StringifiedNonTerminal(const StringifiedNonTerminal & snt2) : StringifiedGrammarSymbol(snt2)
{
}

AkaneLang::StringifiedNonTerminal::StringifiedNonTerminal(const std::string & _nonTerminalName) : StringifiedGrammarSymbol(_nonTerminalName)
{
}

StringifiedNonTerminal * AkaneLang::StringifiedNonTerminal::duplicate_freeNeeded() const
{
	return new StringifiedNonTerminal(*this);
}


/* ==== 产生式 ==== */

string Production::getDescription() const
{
	string desc;
	for (auto symI : leftIndices)
	{
		desc += grammarP->symbols[symI]->getShortDescription();
		desc += " ";
	}
	desc += grammarP->arrowStr;
	for (auto symI : rightIndices)
	{
		desc += " ";
		desc += grammarP->symbols[symI]->getShortDescription();
	}
	return desc;
}

Production::Production(Grammar *_grammarP, vector<SymbolIndex> _leftIndices, vector<SymbolIndex> _rightIndices, Index _indexInGrammar)
	: grammarP(_grammarP), leftIndices(_leftIndices), rightIndices(_rightIndices), indexInGrammar(_indexInGrammar)
{

}


/* ==== LR 项目 ==== */

string AkaneLang::LRItem::getDescription() const
{
	string desc;
	Index i;
	for (i = 0; i < productionP->leftIndices.size(); i++)
	{
		SymbolIndex symI = productionP->leftIndices[i];
		desc += productionP->grammarP->symbols[symI]->getShortDescription();
		desc += " ";
	}

	desc += productionP->grammarP->arrowStr;

	for (i = 0; i < productionP->rightIndices.size(); i++)
	{
		SymbolIndex symI = productionP->rightIndices[i];

		if (dotPosition == i)
		{
			if (i == 0)
				desc += " ";
			desc += "·";
		}
		else
			desc += " ";

		desc += productionP->grammarP->symbols[symI]->getShortDescription();
	}

	if (dotPosition == i)	// productionP->leftIndices.size()
	{
		desc += "·";
	}

	desc += " (";
	desc += productionP->grammarP->symbols[lookAheadIndex]->getShortDescription();
	desc += ")";

	return desc;
}

bool LRItem::isReducing() const
{
	return dotPosition == productionP->rightIndices.size();
}

LRItem::LRItem(Production &_production, size_t _dotPosition, SymbolIndex _lookAheadIndex)
	: productionP(&_production), dotPosition(_dotPosition), lookAheadIndex(_lookAheadIndex)
{

}

LRItem::LRItem(const LRItem &lr2)
	: productionP(lr2.productionP), dotPosition(lr2.dotPosition), lookAheadIndex(lr2.lookAheadIndex)
{

}

bool LRItem::operator==(const LRItem & lri) const

{
	if (productionP != lri.productionP) return false;
	if (dotPosition != lri.dotPosition) return false;
	if (*productionP->grammarP->symbols[lookAheadIndex] != *productionP->grammarP->symbols[lri.lookAheadIndex]) return false;
	return true;
}

LRItem &LRItem::operator=(const LRItem & lri)
{
	productionP = lri.productionP;
	dotPosition = lri.dotPosition;
	lookAheadIndex = lri.lookAheadIndex;

	return *this;
}

/* ==== 文法 ==== */

static const size_t productionsEnds = (std::numeric_limits<size_t>::max)();

AkaneLang::Grammar::Grammar(std::istream & is)
{
	addEOFSymbol();

	is >> std::ws;
	std::getline(is, name);

	is >> arrowStr;

	std::string check;

	// 终结符
	is >> check;
	if (check != "terminal")
		throw AkaneInputValueException("此处应有 terminal, 但读到 %s", check.c_str());

	size_t Tnum;
	is >> Tnum;
	for (size_t i = 0; i < Tnum; i++)
	{
		std::string tStr;
		is >> tStr;
		addSymbol(StringifiedTerminal(tStr));
	}

	// 非终结符
	is >> check;
	if (check != "non-terminal")
		throw AkaneInputValueException("此处应有 non-terminal, 但读到 %s", check.c_str());

	size_t NTnum;
	is >> NTnum;
	for (size_t i = 0; i < NTnum; i++)
	{
		std::string ntStr;
		is >> ntStr;
		addSymbol(StringifiedNonTerminal(ntStr));
	}

	// 产生式
	is >> std::ws;
	while (true)
	{
		std::string thisLine;
		std::getline(is, thisLine);
		std::istringstream thisLineStream(thisLine);

		size_t symbolCount = 0;
		std::string currSymbol;
		std::vector<std::string> leftStrings;
		std::vector<std::string> rightStrings;
		std::vector<std::string> *side = &leftStrings;

		while (true)
		{
			thisLineStream >> std::ws;
			if (thisLineStream.eof())
				if (symbolCount == 0)
					break;	// 空行
				else if (side == &leftStrings)
					throw AkaneInputValueException("文法输入错误: %s %s -> %s %s...", leftStrings.size() > 0 ? leftStrings[0].c_str() : "(empty)", leftStrings.size() > 1 ? leftStrings[1].c_str() : "(empty)", rightStrings.size() > 0 ? rightStrings[0].c_str() : "(empty)", rightStrings.size() > 1 ? rightStrings[1].c_str() : "(empty)"); // 没有箭头, 本行不是合法的产生式
				else
					break; // 产生式正常结束

			thisLineStream >> currSymbol;

			if (currSymbol == arrowStr)
			{
				if (side == &rightStrings)
					throw AkaneInputValueException("文法出现了两次以上箭头: %s %s -> %s %s...", leftStrings.size() > 0 ? leftStrings[0].c_str() : "(empty)", leftStrings.size() > 1 ? leftStrings[1].c_str() : "(empty)", rightStrings.size() > 0 ? rightStrings[0].c_str() : "(empty)", rightStrings.size() > 1 ? rightStrings[1].c_str() : "(empty)");

				if (symbolCount == 0)
				{
					// 产生式第一个读到的就是箭头, 是后续不会再有产生式的标志
					symbolCount = productionsEnds;
					break;
				}

				side = &rightStrings;
				symbolCount = 0;
				continue;
			}

			side->push_back(currSymbol);
			symbolCount++;
		}

		if (symbolCount == productionsEnds)
			break;

		if (symbolCount == 0 && side == &leftStrings)
			continue; // 本行空
	
		addProduction(leftStrings, rightStrings);
	}

	std::string startSymbol;
	is >> startSymbol;

	setStartSymbol(StringifiedNonTerminal(startSymbol));
}

void Grammar::setStartSymbol(const NonTerminal &nt)

{
	auto symFindIt = find_if(symbols.begin(), symbols.end(), [&nt](GrammarSymbol *symP) -> bool {return *symP == nt; const Letter &a = nt; });

	if (symFindIt == symbols.end())
	{
		throw AkaneInputValueException("输入的 %s 没有与之匹配的文法符号.", nt.getShortDescription().c_str());
	}

	startSymbolIndex = distance(symbols.begin(), symFindIt);
}

void Grammar::addEOFSymbol()
{
	eofSymbolIndex = symbols.size();
	symbols.push_back(StringifiedTerminal::eof().duplicate_freeNeeded());
}

void AkaneLang::Grammar::addSymbol(const GrammarSymbol & _symbol)
{
	symbols.push_back(_symbol.duplicate_freeNeeded());
}

inline void AkaneLang::Grammar::addProduction(vector<string> _left, vector<string> _right)
{
	// Left
	vector<SymbolIndex> leftIndices;

	for (auto &symStr : _left)
	{
		auto symFindIt = find_if(symbols.begin(), symbols.end(), [&symStr](GrammarSymbol *symP) -> bool {return symP->getShortDescription() == symStr; });
		if (symFindIt == symbols.end())
		{
			throw AkaneInputValueException("输入的 %s 没有与之匹配的文法符号.", symStr.c_str());
		}

		leftIndices.push_back(distance(symbols.begin(), symFindIt));
	}

	// Right
	vector<SymbolIndex> rightIndices;

	for (auto &symStr : _right)
	{
		auto symFindIt = find_if(symbols.begin(), symbols.end(), [&symStr](GrammarSymbol *symP) -> bool {return symP->getShortDescription() == symStr; });
		if (symFindIt == symbols.end())
		{
			throw AkaneInputValueException("输入的 %s 没有与之匹配的文法符号.", symStr.c_str());
		}

		rightIndices.push_back(distance(symbols.begin(), symFindIt));
	}

	productions.push_back(Production(this, leftIndices, rightIndices, productions.size()));
}

inline void AkaneLang::Grammar::generateFIRST()
{
	FIRST.clear();
	bool somethingChanged = true;
	while (somethingChanged)
	{
		somethingChanged = false;
		for (SymbolIndex symI = 0; symI < symbols.size(); symI++)
		{
			auto &sym = *(symbols[symI]);
			if (FIRST.size() == symI)
				FIRST.push_back(set<SymbolIndex>{});
			auto &thisFIRST = FIRST[symI];
			size_t prevSize = thisFIRST.size();

			if (sym.isTerminal())
			{
				thisFIRST.insert(symI);
			}
			else
			{
				for (auto &prod : productions)
				{
					if (prod.leftIndices.size() == 1 && prod.leftIndices[0] == symI)
					{
						// X->...
						// 这两个分支应该可以用某种巧妙的方式重写, 组合起来
						if (prod.rightIndices.size() == 0)
						{
							// X-><epsilon>
							thisFIRST.insert(epsilonIndex);
						}
						else
						{
							SymbolIndex firstSymIndex = prod.rightIndices[0];
							auto &firstSym = *(symbols[firstSymIndex]);
							if (firstSym.isTerminal())
							{
								// X->a...
								thisFIRST.insert(firstSymIndex);
							}
							else
							{
								// X->Y...

								// 得到 FIRST(Y) - {<epsilon>}
								auto tmpSet = FIRST.size() <= firstSymIndex ? set<SymbolIndex>{} : FIRST[firstSymIndex];
								auto findEpsilonIt = tmpSet.find(epsilonIndex);
								bool haveEpsilon;
								if (haveEpsilon = (findEpsilonIt != tmpSet.end()))
								{
									tmpSet.erase(findEpsilonIt);
								}
								thisFIRST.insert(tmpSet.begin(), tmpSet.end());

								// 若 FIRST(Y) 含 <epsilon> 要将后续的 FIRST 加入
								if (haveEpsilon)
								{
									auto rightSymIIt = prod.rightIndices.begin() + 1;
									for (; rightSymIIt != prod.rightIndices.end(); rightSymIIt++)
									{
										SymbolIndex rightSymI = *rightSymIIt;
										if (symbols[rightSymI]->isTerminal())
										{
											thisFIRST.insert(firstSymIndex);
											// Y[rightSymIIt] 是终结符, 应该跳出循环
											break;
										}

										tmpSet = FIRST[firstSymIndex];
										findEpsilonIt = tmpSet.find(epsilonIndex);
										if (haveEpsilon = (findEpsilonIt != tmpSet.end()))
										{
											tmpSet.erase(findEpsilonIt);
										}
										thisFIRST.insert(tmpSet.begin(), tmpSet.end());
										if (haveEpsilon)
										{
											// Y[rightSymIIt] 没有 <epsilon>, 应该跳出循环
											break;
										}
									}

									if (rightSymIIt == prod.rightIndices.end())
									{
										// 没有跳出循环, 一路都遇到 <epsilon>
										thisFIRST.insert(epsilonIndex);
									}
								}
							}
						}
					}
				}
			}
			if (thisFIRST.size() != prevSize)
				somethingChanged = true;
		}
	}

	return;
}

// 获取一序列文法符号的 FIRST

set<SymbolIndex> AkaneLang::Grammar::getFIRST(vector<SymbolIndex> _letterIndices)
{
	if (FIRST.size() == 0)
		generateFIRST();

	set<SymbolIndex> result;
	auto symIIt = _letterIndices.begin();
	for (; symIIt != _letterIndices.end(); symIIt++)
	{
		SymbolIndex symI = *symIIt;
		set<SymbolIndex> tmpSet(FIRST[symI]);
		auto findEpsilonIt = tmpSet.find(epsilonIndex);
		bool haveEpsilon;
		if (haveEpsilon = (findEpsilonIt != tmpSet.end()))
		{
			// have epsilon
			tmpSet.erase(findEpsilonIt);
		}
		result.insert(tmpSet.begin(), tmpSet.end());

		if (!haveEpsilon)
		{
			break;
		}
	}

	if (symIIt == _letterIndices.end())
	{
		result.insert(epsilonIndex);
	}

	return result;
}

AkaneLang::Grammar::~Grammar()
{
	for (auto symP : symbols)
	{
		delete symP;
	}
}

template<class Out>
inline void Grammar::print(Out & out)
{
	out << "==== 文法打印 ====" << endl;
	out << "名称: " << name << " (" << hex << this << dec << ")" << endl;
	out << endl;
	out << "符号表: " << endl;
	out << endl;
	out << "终结符: ";
	for (auto symP : symbols)
	{
		if (symP->isTerminal() && *symP != StringifiedTerminal::eof())
		{
			out << symP->getShortDescription() << " ";
		}
	}
	out << endl;
	out << "非终结符: ";
	for (auto symP : symbols)
	{
		if (!(symP->isTerminal()))
		{
			out << symP->getShortDescription() << " ";
		}
	}
	out << endl;
	out << endl;

	out << "起始符号: " << symbols[startSymbolIndex]->getShortDescription() << endl << endl;

	out << "生成式:" << endl << endl;

	for (auto &p : productions)
	{
		out << p.getDescription() << endl;
	}

	if (FIRST.size())
	{
		out << endl;
		out << "FIRST 集合: " << endl;
		for (SymbolIndex symI = 0; symI < symbols.size(); symI++)
		{
			out << "FIRST(" << symbols[symI]->getShortDescription() << ") = { ";
			for (auto symI2 : FIRST[symI])
			{
				if (symI2 == epsilonIndex)
					out << "ε " << ", ";
				else
					out << symbols[symI2]->getShortDescription() << ", ";
			}
			out << "}" << endl;
		}
	}
}

template void Grammar::print<std::ostream>(std::ostream &out); 
template void Grammar::print<AkaneUtils::Logger>(AkaneUtils::Logger &out);
