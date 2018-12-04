#pragma once

namespace AkaneLang
{
	typedef size_t StateIndex;

	extern const char *automataStateTagText[3];
	extern const StateIndex invalidStateIndex;

	enum AutomataStateTag
	{
		notAccepted = 0,
		accepted = 1,
		intermediate = 2,
	};

	struct BaseState
	{
		std::string description;
		AutomataStateTag tag;
		std::map<StringifiedLetter, std::set< StateIndex>> delta;

		BaseState() { tag = AutomataStateTag::notAccepted; }

		virtual void startMerge();
		virtual void mergeThisState(const BaseState &_state);
		virtual void endMerge();

		//virtual bool operator==(const BaseState &right) const;
	};

	struct ItemState : public BaseState
	{
		std::vector<LRItem> *lrItemsP;
		std::set<Index> itemIndices;
		 
		ItemState();
		ItemState(std::vector<LRItem> *_lrItemsP, const std::set<Index> &_itemIndices, AutomataStateTag _tag);


		virtual void startMerge() override;
		virtual void mergeThisState(const BaseState &_state) override;
		virtual void endMerge() override;

		virtual bool operator==(const BaseState &right) const/* override*/;
	};

}