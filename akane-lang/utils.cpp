#include "utils.h"
#include <fstream>

using namespace std;
using namespace AkaneUtils;

std::string AkaneUtils::getTimeString(const char *format = "%m%d%H%M%S")
{
	time_t currTime;
	struct tm *timeInfo;
	time(&currTime);
	timeInfo = localtime(&currTime);

	std::ostringstream oss;
	oss << std::put_time(timeInfo, format);
	return oss.str();
}

Logger & AkaneUtils::Logger::getInstance()
{
	static std::string fileName = AKANEUTILS_LOGGER_FILENAME_PREFIX
#ifndef AKANEUTILS_LOGGER_FILENAME_DISABLE_DATE
		+ getTimeString()
#endif
		+ ".log"
		;

	static Logger theLogger(fileName.c_str());
	return theLogger;
}

std::ofstream AkaneUtils::Logger::getLogFile(int mode)
{
	ofstream file(logFileName, mode);
	if (!file.is_open())
	{
		//throw AkaneSystemException("Error: Log file %s can't be open as write.", logFileName);
		return ofstream();
	}
	return file;
}

std::FILE * AkaneUtils::Logger::getLogFile(const char * mode)
{
	FILE *file = fopen(logFileName.c_str(), mode);
	if (!file)
	{
		//throw AkaneSystemException("Error: Log file %s can't be open as write.", logFileName);
	}
	return file;
}

std::ofstream AkaneUtils::Logger::getErrorFile(int mode)
{
	ofstream file(errorFileName, mode);
	if (!file.is_open())
	{
		//throw AkaneSystemException("Error: Log file %s can't be open as write.", errorFileName);
		return ofstream();
	}
	return file;
}

std::FILE * AkaneUtils::Logger::getErrorFile(const char * mode)
{
	FILE *file = fopen(errorFileName.c_str(), mode);
	if (!file)
	{
		//throw AkaneSystemException("Error: Log file %s can't be open as write.", errorFileName);
	}
	return file;
}

AkaneUtils::Logger::Logger(const char * fileName) : errorFileName(fileName), logFileName(fileName)
{
}

AkaneUtils::Logger::Logger(const char * logFileName, const char * errorFileName) : errorFileName(errorFileName), logFileName(logFileName)
{
}

void AkaneUtils::Logger::log(const char * msg_format, ...)
{
	va_list args;
	int result;

	FILE *file = getLogFile("a");
	if (!file)
		return;
	va_start(args, msg_format);
	result = log_insideVa(file, msg_format, args);
	va_end(args);
	if (result < 0)
	{
		throw AkaneSystemException("Error: Writing %s to %s failed.", msg_format, logFileName);
	}
	fclose(file);
}

void AkaneUtils::Logger::error(const char * msg_format, ...)
{
	va_list args;
	int result;

	FILE *logFile = getLogFile("a");
	FILE *errorFile = getErrorFile("a");

	va_start(args, msg_format);
	result = error_insideVa(this->logFileName == this->errorFileName ? nullptr : logFile, errorFile, msg_format, args);
	va_end(args);
	if (result < 0)
	{
		throw AkaneSystemException("Error: Writing %s to %s, %s failed.", msg_format, logFileName, errorFileName);
	}

	logFile && fclose(logFile);
	errorFile && fclose(errorFile);
}

int AkaneUtils::Logger::log_insideVa(FILE *file, const char * msg_format, va_list args)
{

	int result1 = 0, result2 = 0;

	if (printLogToStdout)
	{
		vfprintf(stdout, msg_format, args);
		fprintf(stdout, "\n");
	}

	result1 = vfprintf_s(file, msg_format, args);
	result2 = result1 < 0 ? -1 : fprintf_s(file, "\n");

	return result2;

}

int AkaneUtils::Logger::error_insideVa(FILE *logFile, FILE *errorFile, const char * msg_format, va_list args)
{

	int result1 = 0, result2 = 0;

	if (printErrorToStderr)
	{
		vfprintf(stderr, msg_format, args);
		fprintf(stderr, "\n");
	}

	if (logFile)
	{
		result1 = vfprintf_s(logFile, msg_format, args);
		result2 = result1 < 0 ? -1 : fprintf_s(logFile, "\n");
	}

	if (errorFile)
	{
		result1 = result2 < 0 ? -1 : vfprintf_s(errorFile, msg_format, args);
		result2 = result1 < 0 ? -1 : fprintf_s(errorFile, "\n");
	}

	return result2;

}

Logger & Logger::operator<<(decltype(std::endl<char, std::char_traits<char>>)& e)
{
	std::ofstream f(getLogFile(std::ios::app));
	if (f.is_open())
		f << e;
	if (printLogToStdout)
	{
		cout << e;
	}
	return *this;
}

AkaneException::AkaneException(const char *msg_format, ...)
{
	va_list args;
	va_start(args, msg_format);
	AkaneException::AkaneException(msg_format, args);
	va_end(args);
}

AkaneException::AkaneException(const char *msg_format, va_list args)
{
	char finalMessage[AkaneExceptionMaxBuf];
	vsprintf_s(finalMessage, msg_format, args);
	le(finalMessage);
	this->std::exception::exception(finalMessage);
}