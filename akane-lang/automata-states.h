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

	template <class L>
	struct SimpleState
	{
		static_assert(std::is_base_of_v<Letter, L>, "±ØÐë¼Ì³Ð×Ô Letter");
		std::string description;
		AutomataStateTag tag;
		std::map<L, std::set< StateIndex>> delta;

		SimpleState() { tag = AutomataStateTag::notAccepted; }

		virtual void startMerge();
		virtual void mergeThisState(SimpleState &_state);
		virtual void endMerge();
	};

	template <class L>
	struct ItemState : public SimpleState<L>
	{
		std::vector<LRItem> *lriContainer;
		std::set<Index> itemIndices;

		virtual void startMerge() override;
		virtual void mergeThisState(SimpleState<L> &_state) override;
		virtual void endMerge() override;

		ItemState() : lriContainer(nullptr), itemIndices({}) {}

		ItemState(std::vector<LRItem> *_lriContainer, const std::set<Index> &_itemIndices, AutomataStateTag _tag) : lriContainer(_lriContainer), itemIndices(_itemIndices) {
			SimpleState<L>::tag = _tag;
			for (Index i : itemIndices)
			{
				SimpleState<L>::description += lriContainer->at(i).getDescription();
				SimpleState<L>::description += "\n";
			}
			if(SimpleState<L>::description.size())
				SimpleState<L>::description.pop_back();
		}
	};



	template<class L>
	inline void SimpleState<L>::startMerge()
	{
		description = "";
	}

	template<class L>
	inline void SimpleState<L>::mergeThisState(SimpleState & _state)
	{
		description += _state.description;
		description += " | ";
	}
	template<class L>
	inline void SimpleState<L>::endMerge()
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
	template<class L>
	inline void ItemState<L>::startMerge()
	{
		this->SimpleState<L>::startMerge();
		itemIndices.clear();
	}
	template<class L>
	inline void ItemState<L>::mergeThisState(SimpleState<L> & _state)
	{
		this->SimpleState<L>::mergeThisState(_state);

		ItemState<L> &_state2 = dynamic_cast<ItemState<L> &>(_state);
		if (this->lriContainer != nullptr && this->lriContainer != _state2.lriContainer)
		{
			throw AkaneRuntimeException("lriContainer Inconsistet");
		}
		this->lriContainer = _state2.lriContainer;
		auto &itsPI = _state2.itemIndices;
		itemIndices.insert(itsPI.begin(), itsPI.end());
	}
	template<class L>
	inline void ItemState<L>::endMerge()
	{
		this->SimpleState<L>::endMerge();
	}
}