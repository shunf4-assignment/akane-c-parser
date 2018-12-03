#pragma once
#include "akane-parse.h"
namespace AkaneParse
{
	const size_t TreePrintingIndent = 4;
	class ParseTree;

	class ParseTreeNode
	{
	public:
		AkaneLang::GrammarSymbol *symbol;
		std::vector<Index> children;

		ParseTreeNode(const ParseTreeNode &ptn)
		{
			symbol = ptn.symbol ? ptn.symbol->duplicate_freeNeeded() : nullptr;
			children = ptn.children;
		}

		ParseTreeNode(const AkaneLang::GrammarSymbol &gs)
		{
			symbol = gs.duplicate_freeNeeded();
		}

		ParseTreeNode() : symbol(nullptr)
		{
		}

		void operator=(const AkaneLang::GrammarSymbol &gs)
		{
			symbol = gs.duplicate_freeNeeded();
		}

		template<class Out>
		void print(size_t indentLevel, ParseTree &tree, Out &out, std::vector<bool> &levelsEnd);

		~ParseTreeNode()
		{
			if (symbol)
				delete symbol;
		}
	};

	class ParseTree
	{
	public:
		Index rootIndex;
		std::vector<ParseTreeNode> nodes;

		size_t addNode(const AkaneLang::GrammarSymbol &gs)
		{
			nodes.push_back(ParseTreeNode());
			nodes.back() = gs;
			return nodes.size() - 1;
		}

		template<class Out>
		void print(Out &out)
		{
			std::vector<bool> levelsEnd{};
			nodes[rootIndex].print(0, *this, out, levelsEnd);
		}
	};


	template<class Out>
	inline void ParseTreeNode::print(size_t indentLevel, ParseTree & tree, Out & out, std::vector<bool> &levelsEnd)
	{
		for (size_t i = 0; i < indentLevel; i++)
		{
			out << ((i == indentLevel - 1) ? '+' : levelsEnd[i] ? ' ' : '|') << std::setfill((i == indentLevel - 1) ? '-' : ' ') << std::setw(TreePrintingIndent - 1) << "" << std::setfill(' ');
		}
		out << symbol->getLongDescription() << std::endl;
		if (levelsEnd.size() <= indentLevel)
		{
			levelsEnd.push_back(false);
		}
		for (auto it = children.begin(); it != children.end(); it++)
		{
			levelsEnd[indentLevel] = (it + 1 == children.end());
			tree.nodes[*it].print(indentLevel + 1, tree, out, levelsEnd);
		}
	}
}
