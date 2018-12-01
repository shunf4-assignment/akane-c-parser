#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <exception>
#include <cstdarg>

#ifndef AKANEUTILS_LOGGER_FILENAME_PREFIX
#define AKANEUTILS_LOGGER_FILENAME_PREFIX "logs/akane-log-"
#endif

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
	std::string getTimeString(const char *format);

    class Logger
    {
    public:
		static Logger &getInstance();

		bool printLogToStdout = false;
		bool printErrorToStderr = true;

		std::string logFileName;
		std::string errorFileName;
        std::ofstream getLogFile(int mode);
        std::FILE *getLogFile(const char *mode);

		std::ofstream getErrorFile(int mode);
		std::FILE *getErrorFile(const char *mode);

		Logger(const char *fileName);
		Logger(const char *logFileName, const char *errorFileName);
        void log(const char *msg_format, ...);
        void error(const char *msg_format, ...);

		template <class T>
		Logger &operator<<(const T&e);

		Logger &operator<<(decltype(std::endl<char, std::char_traits<char>>) &e);
        //void error(const char *msg_format, va_list args);
	private:
		int log_insideVa(FILE *file, const char * msg_format, va_list args);
		int error_insideVa(FILE *logFile, FILE *errorFile, const char * msg_format, va_list args);
		
    };

	template<class T>
	Logger & Logger::operator<<(const T & e)
	{
		std::ofstream f(getLogFile(std::ios::app));
		if (f.is_open())
			f << e;
		if (printLogToStdout)
		{
			std::cout << e;
		}
		return *this;
	}
}

