#pragma once

DEFINE_AKANE_EXCEPTION(Runtime)

namespace AkaneLang
{
	// "��ĸ" �Ǳ���������һ�з��� (�Զ����еķ���, �ķ��е��ս��/���ս��) ��ͳ��.
	struct Letter
	{
		static const Letter &epsilon() = delete;
		static const Letter &elseLetter() = delete;
		virtual bool operator<(const Letter &r) const { return false; }
		virtual bool operator>(const Letter &r) const { return false; }
		virtual bool operator==(const Letter &r) const { return false; }
		virtual bool operator<=(const Letter &r) const { return false; }
		virtual bool operator>=(const Letter &r) const { return false; }
		virtual bool operator!=(const Letter &r) const { return false; }
		virtual std::string getLetterDescription() const = 0;
		virtual std::string getLongDescription() const = 0;
	};

	// "LetterString" : 
	struct LetterString : public Letter
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
		static const LetterString &elseLetter();
		virtual bool operator<(const Letter &r) const override;
		virtual bool operator>(const Letter &r) const override;
		virtual bool operator==(const Letter &r) const override;
		virtual bool operator<=(const Letter &r) const override;
		virtual bool operator>=(const Letter &r) const override;
		virtual bool operator!=(const Letter &r) const override;

		virtual std::string getLetterDescription() const { return escape(text); }
		virtual std::string getLongDescription() const { return escape(text); }
	};

	struct LetterGenerator
	{
		virtual const Letter &next_freeNeeded() = 0;
		virtual const Letter &peek_freeNeeded() = 0;
	};

	struct LetterStringGenerator : public LetterGenerator
	{
		// ��ر�֤����������
		std::istream &s;
		std::streampos lastPos;
		std::stringstream currWordStream;
		virtual const LetterString &next_freeNeeded();
		virtual const LetterString &peek_freeNeeded();
		template <typename T>
		void dumpCurrWord(T &dest);
		// ��ر�֤ _s ����������
		LetterStringGenerator(std::istream &_s);
	};

	template<typename T>
	inline void LetterStringGenerator::dumpCurrWord(T & dest)
	{
		currWordStream.seekg(0, std::ios::beg);
		currWordStream >> dest;
		lastPos = s.tellg();
	}

	template<>
	inline void LetterStringGenerator::dumpCurrWord(std::string & dest)
	{
		currWordStream.seekg(0, std::ios::beg);
		dest = currWordStream.str();
		currWordStream.str("");
		lastPos = s.tellg();
	}
}