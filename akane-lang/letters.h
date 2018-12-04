#pragma once

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
	struct TokenizedLetter;
	struct StringifiedLetter : public virtual Letter
	{
		std::string name;
		StringifiedLetter();
		StringifiedLetter(const StringifiedLetter &_l);
		StringifiedLetter(const std::string &_name);
		StringifiedLetter(const TokenizedLetter &lt);
		
		StringifiedLetter &operator=(const StringifiedLetter &right);

		static const StringifiedLetter &epsilon();
		static const StringifiedLetter &elseLetter();
		static const StringifiedLetter & eof();

		virtual std::string getUniqueName() const override;
		virtual std::string getShortDescription() const;
		virtual std::string getLongDescription() const;
		virtual StringifiedLetter *duplicate_freeNeeded() const;

		virtual ~StringifiedLetter() {};
	};

	struct LetterGenerator
	{
		virtual const Letter *next_freeNeeded() = 0;
		virtual const Letter *peek_freeNeeded() = 0;
		virtual void ignoreOne() = 0;
	};

	struct StreamedLetterGenerator : public LetterGenerator
	{
		std::istream *streamP;
		std::streampos lastPos;
		std::stringstream currLexemeStream;
		virtual const StringifiedLetter *next_freeNeeded();
		virtual const StringifiedLetter *peek_freeNeeded();
		virtual void ignoreOne();

		template <typename T>
		void dumpCurrLexeme(T &dest);

		StreamedLetterGenerator(std::istream &_s);
	};

	template<typename T>
	inline void StreamedLetterGenerator::dumpCurrLexeme(T & dest)
	{
		currLexemeStream.seekg(0, std::ios::beg);
		currLexemeStream >> dest;
		lastPos = streamP->tellg();
	}

	template<>
	inline void StreamedLetterGenerator::dumpCurrLexeme(std::string & dest)
	{
		currLexemeStream.seekg(0, std::ios::beg);
		dest = currLexemeStream.str();
		currLexemeStream.str("");
		lastPos = streamP->tellg();
	}
}