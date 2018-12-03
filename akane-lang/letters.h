#pragma once

DEFINE_AKANE_EXCEPTION(Runtime)

namespace AkaneLang
{
	/*
	
	Letter

		����ʽ���������з��ŵĻ���.
		
			�����˱Ƚ������;
			getShortDescription, getLongDescription : ��ȡ�˿ɶ����ı��Ľӿ�;
			getUniqueName: ��ȡ�ַ�����ʽ������������ Letter ��Ψһ����;
			duplicate/duplicate_freeNeeded : ����һ��ָ�� Letter ���󸱱���ָ��.
	
	*/
	struct Letter
	{
		virtual bool operator<(const Letter &l) const { return getUniqueName() < l.getUniqueName(); }
		virtual bool operator>(const Letter &l) const { return getUniqueName() > l.getUniqueName(); }
		virtual bool operator<=(const Letter &l) const { return getUniqueName() <= l.getUniqueName(); }
		virtual bool operator>=(const Letter &l) const { return getUniqueName() >= l.getUniqueName(); }
		virtual bool operator==(const Letter &l) const { return getUniqueName() == l.getUniqueName(); }
		virtual bool operator!=(const Letter &l) const { return getUniqueName() != l.getUniqueName(); }
		virtual std::string getShortDescription() const = 0;
		virtual std::string getLongDescription() const = 0;
		virtual std::string getUniqueName() const = 0;
		virtual Letter *duplicate_freeNeeded() const = 0;
		virtual std::unique_ptr<Letter> duplicate() const = 0;

		virtual ~Letter() = 0;
	};

	/*

	StringifiedLetter

		���ڲ��洢���ַ�����ʽ�������������ݵ� Letter ����, ������ TokenizedLetter, �������ڴ�.

			�����˱Ƚ������;
			getShortDescription, getLongDescription : ��ȡ�˿ɶ����ı��Ľӿ�;
			getUniqueName: ��ȡ�ַ�����ʽ������������ Letter ��Ψһ����;
			duplicate/duplicate_freeNeeded : ����һ��ָ�� Letter ���󸱱���ָ��.

	*/
	struct StringifiedLetter : public virtual Letter
	{
		std::string name;
		StringifiedLetter() : Letter()
		{
			name = "(invalid)";
		}
		StringifiedLetter(const StringifiedLetter &_l) : Letter(_l)
		{
			name = _l.name;
		}
		StringifiedLetter(const std::string &_name);

		static const StringifiedLetter &epsilon();
		static const StringifiedLetter &elseLetter();

		virtual std::string getUniqueName() const override { return name; }
		virtual std::string getShortDescription() const { return escape(name); }
		virtual std::string getLongDescription() const { return escape(name); }
		virtual StringifiedLetter *duplicate_freeNeeded() const { return new StringifiedLetter(*this); }

		virtual ~StringifiedLetter() {};
	};

	struct LetterGenerator
	{
		virtual std::unique_ptr<Letter> next() = 0;
		virtual std::unique_ptr<Letter> peek() = 0;
		virtual const Letter *next_freeNeeded() = 0;
		virtual const Letter *peek_freeNeeded() = 0;
		virtual void ignoreOne() = 0;
	};

	struct StreamedLetterGenerator : public LetterGenerator
	{
		std::istream *streamP;
		std::streampos lastPos;
		std::stringstream *currTokenStreamP;
		virtual std::unique_ptr<NamedLetter> next();
		virtual std::unique_ptr<NamedLetter> peek();
		virtual const NamedLetter &next_freeNeeded();
		virtual const NamedLetter &peek_freeNeeded();
		template <typename T>
		void dumpCurrWord(T &dest);
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