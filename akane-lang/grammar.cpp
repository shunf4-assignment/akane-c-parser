#include "stdafx.h"
#include "akane-lang.h"

using namespace AkaneLang;
using namespace std;

/* ==== �ķ�����Ԫ�� ==== */

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


/* ==== ����ʽ ==== */

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


/* ==== LR ��Ŀ ==== */

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
			desc += "��";
		}
		else
			desc += " ";

		desc += productionP->grammarP->symbols[symI]->getShortDescription();
	}

	if (dotPosition == i)	// productionP->leftIndices.size()
	{
		desc += "��";
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

/* ==== �ķ� ==== */

static const size_t productionsEnds = (std::numeric_limits<size_t>::max)();

AkaneLang::Grammar::Grammar(std::istream & is)
{
	addEOFSymbol();

	is >> std::ws;
	std::getline(is, name);

	is >> arrowStr;

	std::string check;

	// �ս��
	is >> check;
	if (check != "terminal")
		throw AkaneInputValueException("�˴�Ӧ�� terminal, ������ %s", check.c_str());

	size_t Tnum;
	is >> Tnum;
	for (size_t i = 0; i < Tnum; i++)
	{
		std::string tStr;
		is >> tStr;
		addSymbol(StringifiedTerminal(tStr));
	}

	// ���ս��
	is >> check;
	if (check != "non-terminal")
		throw AkaneInputValueException("�˴�Ӧ�� non-terminal, ������ %s", check.c_str());

	size_t NTnum;
	is >> NTnum;
	for (size_t i = 0; i < NTnum; i++)
	{
		std::string ntStr;
		is >> ntStr;
		addSymbol(StringifiedNonTerminal(ntStr));
	}

	// ����ʽ
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
					break;	// ����
				else if (side == &leftStrings)
					throw AkaneInputValueException("�ķ��������: %s %s -> %s %s...", leftStrings.size() > 0 ? leftStrings[0].c_str() : "(empty)", leftStrings.size() > 1 ? leftStrings[1].c_str() : "(empty)", rightStrings.size() > 0 ? rightStrings[0].c_str() : "(empty)", rightStrings.size() > 1 ? rightStrings[1].c_str() : "(empty)"); // û�м�ͷ, ���в��ǺϷ��Ĳ���ʽ
				else
					break; // ����ʽ��������

			thisLineStream >> currSymbol;

			if (currSymbol == arrowStr)
			{
				if (side == &rightStrings)
					throw AkaneInputValueException("�ķ��������������ϼ�ͷ: %s %s -> %s %s...", leftStrings.size() > 0 ? leftStrings[0].c_str() : "(empty)", leftStrings.size() > 1 ? leftStrings[1].c_str() : "(empty)", rightStrings.size() > 0 ? rightStrings[0].c_str() : "(empty)", rightStrings.size() > 1 ? rightStrings[1].c_str() : "(empty)");

				if (symbolCount == 0)
				{
					// ����ʽ��һ�������ľ��Ǽ�ͷ, �Ǻ����������в���ʽ�ı�־
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
			continue; // ���п�
	
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
		throw AkaneInputValueException("����� %s û����֮ƥ����ķ�����.", nt.getShortDescription().c_str());
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
			throw AkaneInputValueException("����� %s û����֮ƥ����ķ�����.", symStr.c_str());
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
			throw AkaneInputValueException("����� %s û����֮ƥ����ķ�����.", symStr.c_str());
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
						// ��������֧Ӧ�ÿ�����ĳ������ķ�ʽ��д, �������
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

								// �õ� FIRST(Y) - {<epsilon>}
								auto tmpSet = FIRST.size() <= firstSymIndex ? set<SymbolIndex>{} : FIRST[firstSymIndex];
								auto findEpsilonIt = tmpSet.find(epsilonIndex);
								bool haveEpsilon;
								if (haveEpsilon = (findEpsilonIt != tmpSet.end()))
								{
									tmpSet.erase(findEpsilonIt);
								}
								thisFIRST.insert(tmpSet.begin(), tmpSet.end());

								// �� FIRST(Y) �� <epsilon> Ҫ�������� FIRST ����
								if (haveEpsilon)
								{
									auto rightSymIIt = prod.rightIndices.begin() + 1;
									for (; rightSymIIt != prod.rightIndices.end(); rightSymIIt++)
									{
										SymbolIndex rightSymI = *rightSymIIt;
										if (symbols[rightSymI]->isTerminal())
										{
											thisFIRST.insert(firstSymIndex);
											// Y[rightSymIIt] ���ս��, Ӧ������ѭ��
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
											// Y[rightSymIIt] û�� <epsilon>, Ӧ������ѭ��
											break;
										}
									}

									if (rightSymIIt == prod.rightIndices.end())
									{
										// û������ѭ��, һ·������ <epsilon>
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

// ��ȡһ�����ķ����ŵ� FIRST

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
	out << "==== �ķ���ӡ ====" << endl;
	out << "����: " << name << " (" << hex << this << dec << ")" << endl;
	out << endl;
	out << "���ű�: " << endl;
	out << endl;
	out << "�ս��: ";
	for (auto symP : symbols)
	{
		if (symP->isTerminal() && *symP != StringifiedTerminal::eof())
		{
			out << symP->getShortDescription() << " ";
		}
	}
	out << endl;
	out << "���ս��: ";
	for (auto symP : symbols)
	{
		if (!(symP->isTerminal()))
		{
			out << symP->getShortDescription() << " ";
		}
	}
	out << endl;
	out << endl;

	out << "��ʼ����: " << symbols[startSymbolIndex]->getShortDescription() << endl << endl;

	out << "����ʽ:" << endl << endl;

	for (auto &p : productions)
	{
		out << p.getDescription() << endl;
	}

	if (FIRST.size())
	{
		out << endl;
		out << "FIRST ����: " << endl;
		for (SymbolIndex symI = 0; symI < symbols.size(); symI++)
		{
			out << "FIRST(" << symbols[symI]->getShortDescription() << ") = { ";
			for (auto symI2 : FIRST[symI])
			{
				if (symI2 == epsilonIndex)
					out << "�� " << ", ";
				else
					out << symbols[symI2]->getShortDescription() << ", ";
			}
			out << "}" << endl;
		}
	}
}

template void Grammar::print<std::ostream>(std::ostream &out); 
template void Grammar::print<AkaneUtils::Logger>(AkaneUtils::Logger &out);
