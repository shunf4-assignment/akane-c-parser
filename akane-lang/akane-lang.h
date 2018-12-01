#pragma once
#define AKANE_EXCEPTION_PRINT
#include <vector>
#include <string>
#include <map>
#include <set>
#include "utils.h"

#define GET_ASSIGN_AND_FREE(type, result, thing) \
	type & result ## R = dynamic_cast< type &>(thing);\
	type result = result ## R;\
	delete &result##R;

DEFINE_AKANE_EXCEPTION(GetEOF)
DEFINE_AKANE_EXCEPTION(Lang)
DEFINE_AKANE_EXCEPTION(InputValue)

namespace AkaneLex {
	struct Token;
}

namespace AkaneLang
{
	// 字母 = 终结符 = FA 中的符号
	struct Letter
	{
		static const Letter &epsilon();
		virtual bool operator<(const Letter &r) const { return false; }
		virtual bool operator>(const Letter &r) const { return false; }
		virtual bool operator==(const Letter &r) const { return false; }
		virtual bool operator<=(const Letter &r) const { return false; }
		virtual bool operator>=(const Letter &r) const { return false; }
		virtual bool operator!=(const Letter &r) const { return false; }
		virtual std::string getValue() const = 0;
		virtual std::string getDescription() const = 0;
		
	};

	struct LetterString : virtual public Letter
	{
		std::string text;
		LetterString() : Letter()
		{
			text = "(invalid)";
		}
		LetterString(const LetterString &_l) : Letter(_l)
		{
			text = _l.text;
		}
		LetterString(const std::string &);

		static const LetterString &epsilon();
		virtual bool operator<(const Letter &r) const;
		virtual bool operator>(const Letter &r) const;
		virtual bool operator==(const Letter &r) const;
		virtual bool operator<=(const Letter &r) const;
		virtual bool operator>=(const Letter &r) const;
		virtual bool operator!=(const Letter &r) const;

		static std::string unescape(const std::string &str);
		static LetterString unescape(const LetterString &l);
		static std::string escape(const std::string &orig);

		virtual std::string getValue() const { return text; }
		virtual std::string getDescription() const { return escape(text); }
	};

	struct LetterGenerator
	{
		virtual const Letter &next_freeNeeded() = 0;
		virtual const Letter &peek_freeNeeded() = 0;
	};


	struct LetterStringGenerator : public virtual LetterGenerator
	{
		// 务必保证在生存期内
		std::istream &s;
		std::streampos lastPos;
		std::stringstream currWord;
		virtual const LetterString &next_freeNeeded();
		virtual const LetterString &peek_freeNeeded();
		template <typename T>
		void dumpCurrWord(T &dest);
		// 务必保证 _s 在生存期内
		LetterStringGenerator(std::istream &_s);
	};


	typedef size_t StateIndex;

	extern const char *automataStateTagText[3];

	enum AutomataStateTag
	{
		notAccepted = 0,
		accepted = 1,
		intermediate = 2,
	};

	template <class L>
	struct SimpleState
	{
		static_assert(std::is_base_of_v<Letter, L>, "必须继承自 Letter");
		std::string description;
		AutomataStateTag tag;
		std::map<L, std::set< StateIndex>> delta;
	};

	template <class L, template <class LL> class AutomataState>
	class NFA
	{
	public:
		static_assert(std::is_base_of_v<Letter, L>, "必须继承自 Letter");
		std::vector< L> alphabet;
		std::vector< AutomataState<L>> states;
		LetterGenerator *letterGenP;
		std::map<L, L> tagifyMap;
		std::string name;

		NFA(const std::vector< L> &_alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator *_letterGenP);
		NFA();
		virtual std::string typeStr() const;
		void addState(AutomataState< L> state);
		void loadAlphabet(const std::vector< L>& _alphabet);
		void loadtagifyMap(const std::map<L, L> &_tagifyMap);
		void loadStates(const std::vector< AutomataState< L>> &_states);
		void loadDeltas(std::vector< std::map< L, std::set<StateIndex>>> &_indexedDeltas);
		std::string makeDescription(std::set< StateIndex> &_stateSet);
		bool haveAcceptedState(std::set< StateIndex> &_stateSet);
		bool haveIntermediateState(std::set<StateIndex>& _stateSet);
		void setInitialStateIndex(size_t _initialStateIndex){initialStateIndex = _initialStateIndex;}
		size_t getInitialStateIndex()	{return initialStateIndex;}
		std::set<StateIndex> epsilonClosure(const std::set<StateIndex> &ss);
		template <class Out> void print(Out &out);
		virtual void validate();
		size_t initialStateIndex;
	};

	// DFA 的 states 最后一个 state 是自动创建的陷阱 state, 用来在接收到无法识别的字母时跳转
	template <class L, template <class LL> class AutomataState>
	class DFA : public virtual NFA<L, AutomataState>
	{
	public:
		DFA(const std::vector< L> &_alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator *_letterGenP);
		DFA(NFA<L, AutomataState> &nfa);
		virtual void validate();
		void oneStep();
		void oneStep_peek();
		AutomataStateTag peekStateTag();
		void reset();
		virtual std::string typeStr() const;
		void setCurrStateIndex(size_t _currStateIndex){currStateIndex = _currStateIndex;}
		size_t getCurrStateIndex(){return currStateIndex;}
		AutomataStateTag nowStateTag();
	private:
		size_t currStateIndex;
	};

	template <class L, template <class LL> class AutomataState>
	class LexDFA : public virtual DFA<L, AutomataState>
	{
	public:
		LexDFA(const std::vector< L> &_alphabet, const std::map<L, L> &_tagifyMap, LetterGenerator *_letterGenP);
		LexDFA(NFA<L, AutomataState> &nfa);
	};


	template<typename T>
	inline void LetterStringGenerator::dumpCurrWord(T & dest)
	{
		currWord.seekg(0, std::ios::beg);
		currWord >> dest;
		lastPos = s.tellg();
	}

	template<>
	inline void LetterStringGenerator::dumpCurrWord(std::string & dest)
	{
		currWord.seekg(0, std::ios::beg);
		dest = currWord.str();
		currWord.str("");
		lastPos = s.tellg();
	}
}
