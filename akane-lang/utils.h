#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <exception>
#include <cstdarg>

#define AKANEUTILS_DO_LOG

#ifndef AKANEUTILS_DO_NOT_DEFINE_LG
#define logger AkaneUtils::Logger::getInstance()
#define loggerP (&AkaneUtils::Logger::getInstance())
#define lg logger.log
#define le logger.error
#endif

typedef size_t Index;

class AkaneException : public std::exception
{
	static const int AkaneExceptionMaxBuf = 100;
public:
	AkaneException(const char *msg_format, ...);
	AkaneException(const char *msg_format, va_list args);
};

#define DEFINE_AKANE_EXCEPTION(name) class Akane##name##Exception : public AkaneException\
{\
public:\
	Akane##name##Exception(const char *msg_format, ...) : AkaneException(msg_format, (va_start(__args, msg_format), __args))\
	{\
		va_end(__args);\
	}\
private:\
	va_list __args;\
};

DEFINE_AKANE_EXCEPTION(System)

namespace AkaneUtils
{
	extern std::string globalLogFileName;
	extern std::string globalErrorFileName;

	std::string getTimeString(const char *format);

	int constexpr writingLogStream = 0;
	int constexpr writingLogFILE = 1;
	int constexpr writingErrorStream = 2;
	int constexpr writingErrorFILE = 3;

    class Logger
    {
    public:
		static Logger &getInstance();

		bool printLogToStdout = false;
		bool printErrorToStderr = true;

		int nowState;

		FILE *openedLogFILE;
		std::ofstream *openedLogStream;
		FILE *openedErrorFILE;
		std::ofstream *openedErrorStream;

		std::string logFileName;
		std::string errorFileName;
        std::ofstream &getLogStream();
        std::FILE *getLogFILE();

		std::ofstream &getErrorStream();
		std::FILE *getErrorFILE();

		Logger(const char *fileName);
		Logger(const char *logFileName, const char *errorFileName);
        void log(const char *msg_format, ...);
        void error(const char *msg_format, ...);
		void active(int destState);
		void closeAll() { closeLog(); closeError(); }
		void closeLog();
		void closeError();


		template <class T>
		Logger &operator<<(const T&e)
		{
#ifdef AKANEUTILS_DO_LOG
			std::ofstream &f = (getLogStream());
			if (f.is_open())
				f << e << std::flush;
			if (printLogToStdout)
			{
				std::cout << e << std::flush;
			}
#endif
			return *this;
		}

		Logger &operator<<(decltype(std::endl<char, std::char_traits<char>>) &e)
		{
#ifdef AKANEUTILS_DO_LOG
			std::ofstream &f = getLogStream();
			if (f.is_open())
				f << e << std::flush;
			if (printLogToStdout)
			{
				std::cout << e << std::flush;
			}
#endif
			return *this;
		}

		template <class T>
		Logger &operator<(const T&e)
		{
#ifdef AKANEUTILS_DO_LOG
			std::ofstream &f = (getErrorStream());
			if (f.is_open())
				f << e << std::flush;
			if (printErrorToStderr)
			{
				std::cerr << e << std::flush;
			}
			return *this;
#endif
		}

		Logger &operator<(decltype(std::endl<char, std::char_traits<char>>) &e)
		{
#ifdef AKANEUTILS_DO_LOG
			std::ofstream &f = getErrorStream();
			if (f.is_open())
				f << e << std::flush;
			if (printErrorToStderr)
			{
				std::cerr << e << std::flush;
			}
			return *this;
#endif
		}

		~Logger();
        //void error(const char *msg_format, va_list args);
	private:
		int log_insideVa(FILE *file, const char * msg_format, va_list args);
		int error_insideVa(FILE *logFile, FILE *errorFile, const char * msg_format, va_list args);
		
    };

}

