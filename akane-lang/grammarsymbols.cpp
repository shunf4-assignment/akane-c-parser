#include "stdafx.h"
#include "akane-lang.h"

std::string AkaneLang::Production::getDescription()
{
	std::string desc;
	for (auto symI : leftIndices)
	{
		desc += grammarP->symbols[symI]->getSymbolDescription();
		desc += " ";
	}
	desc += grammarP->arrowStr;
	for (auto symI : rightIndices)
	{
		desc += " ";
		desc += grammarP->symbols[symI]->getSymbolDescription();
	}
	return desc;
}

std::string AkaneLang::LRItem::getDescription()
{
	std::string desc;
	Index i;
	for (i = 0; i < productionP->leftIndices.size(); i++)
	{
		SymbolIndex symI = productionP->leftIndices[i];
		desc += productionP->grammarP->symbols[symI]->getSymbolDescription();
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
			desc += "¡¤";
		}
		else
			desc += " ";

		desc += productionP->grammarP->symbols[symI]->getSymbolDescription();
	}

	if (dotPosition == i)	// productionP->leftIndices.size()
	{
		desc += "¡¤";
	}

	desc += " (";
	desc += productionP->grammarP->symbols[lookAheadIndex]->getSymbolDescription();
	desc += ")";

	return desc;
}

bool AkaneLang::LRItem::operator==(const LRItem & lri)

{
	if (productionP != lri.productionP) return false;
	if (dotPosition != lri.dotPosition) return false;
	if (*productionP->grammarP->symbols[lookAheadIndex] != *productionP->grammarP->symbols[lri.lookAheadIndex]) return false;
	return true;
}

AkaneLang::LRItem & AkaneLang::LRItem::operator=(const LRItem & lri)
{
	productionP = lri.productionP;
	dotPosition = lri.dotPosition;
	lookAheadIndex = lri.lookAheadIndex;

	return *this;
}

AkaneLang::NamedGrammarSymbol::NamedGrammarSymbol(const LetterToken & lt)
	: NamedGrammarSymbol(lt.getUniqueName()) {}