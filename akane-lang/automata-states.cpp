#include "stdafx.h"
#include "akane-lang.h"
using namespace std;
using namespace AkaneLang;


inline void BaseState::startMerge()
{
	description = "";
}

inline void BaseState::mergeThisState(const BaseState & _state)
{
	description += _state.description;
	description += " | ";
}

inline void BaseState::endMerge()
{
	if (description.length() == 0)
	{
		description = "(empty)";
	}
	else
	{
		description.pop_back();
		description.pop_back();
		description.pop_back();
	}
}

/*/
inline bool BaseState::operator==(const BaseState &right) const
{
	if (tag != right.tag)
		return false;
	if (delta != right.delta)
		return false;
	return true;
}*/

AkaneLang::ItemState::ItemState()
 : lrItemsP(nullptr), itemIndices({}) {}

AkaneLang::ItemState::ItemState(std::vector<LRItem>* _lrItemsP, const std::set<Index>& _itemIndices, AutomataStateTag _tag)
	: lrItemsP(_lrItemsP), itemIndices(_itemIndices) {
	BaseState::tag = _tag;
	for (Index i : itemIndices)
	{
		BaseState::description += lrItemsP->at(i).getDescription();
		BaseState::description += "\n";
	}
	if (BaseState::description.size())
		BaseState::description.pop_back();
}

inline void ItemState::startMerge()
{
	this->BaseState::startMerge();
	itemIndices.clear();
}

inline void ItemState::mergeThisState(const BaseState & _state)
{
	this->BaseState::mergeThisState(_state);

	const ItemState &_stateCast = dynamic_cast<const ItemState &>(_state);
	if (this->lrItemsP != nullptr && this->lrItemsP != _stateCast.lrItemsP)
	{
		throw AkaneRuntimeException("lrItemsP Inconsistet");
	}
	this->lrItemsP = _stateCast.lrItemsP;
	auto &itsPI = _stateCast.itemIndices;
	itemIndices.insert(itsPI.begin(), itsPI.end());
}

inline void ItemState::endMerge()
{
	this->BaseState::endMerge();
}

inline bool ItemState::operator==(const BaseState &right) const
{
	/*if (!BaseState::operator==(right))
		return false;*/
	if (tag != right.tag)
		return false;

	auto &rightCast = dynamic_cast<const ItemState &>(right);
	if (lrItemsP != rightCast.lrItemsP)
		return false;

	if (itemIndices.size() > 1 || rightCast.itemIndices.size() > 1)
		throw AkaneNotImplementedException("");

	if (itemIndices.size() != 0 && rightCast.itemIndices.size() != 0)
		if (!(lrItemsP->at(*itemIndices.begin()) == lrItemsP->at(*rightCast.itemIndices.begin())))
			return false;
		else
			;
	else if (itemIndices.size() != 0 || rightCast.itemIndices.size() != 0)
		return false;

	return true;
}
