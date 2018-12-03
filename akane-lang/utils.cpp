#include "stdafx.h"
#include "utils.h"
#include <fstream>

using namespace std;
using namespace AkaneUtils;

std::string AkaneUtils::globalLogFileName = "";
std::string AkaneUtils::globalErrorFileName = "";

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
	static Logger theLogger(globalLogFileName.c_str(), globalErrorFileName.c_str());
	return theLogger;
}

std::ofstream &AkaneUtils::Logger::getLogStream()
{
	active(writingLogStream);
	return *openedLogStream;
}

std::FILE * AkaneUtils::Logger::getLogFILE()
{
	active(writingLogFILE);
	return openedLogFILE;
}

std::ofstream &AkaneUtils::Logger::getErrorStream()
{
	active(writingErrorStream);
	return *openedErrorStream;
}

std::FILE * AkaneUtils::Logger::getErrorFILE()
{
	active(writingErrorFILE);
	return openedErrorFILE;
}

AkaneUtils::Logger::Logger(const char * fileName) : errorFileName(fileName), logFileName(fileName) , openedLogFILE(nullptr), openedLogStream(nullptr), openedErrorFILE(nullptr), openedErrorStream(nullptr), nowState(-1)
{
}

AkaneUtils::Logger::Logger(const char * logFileName, const char * errorFileName) : errorFileName(errorFileName), logFileName(logFileName), openedLogFILE(nullptr), openedLogStream(nullptr), openedErrorFILE(nullptr), openedErrorStream(nullptr), nowState(-1)
{
}

void AkaneUtils::Logger::log(const char * msg_format, ...)
{
#ifdef AKANEUTILS_DO_LOG
	va_list args;
	int result;

	FILE *file = getLogFILE();
	if (!file)
		return;
	va_start(args, msg_format);
	result = log_insideVa(file, msg_format, args);
	va_end(args);
	if (result < 0)
	{
		throw AkaneSystemException("Error: Writing %s to %s failed.", msg_format, logFileName);
	}
	fflush(file);
#endif
}

void AkaneUtils::Logger::error(const char * msg_format, ...)
{
#ifdef AKANEUTILS_DO_LOG
	va_list args;
	int result;

	va_start(args, msg_format);
	if (this->logFileName == this->errorFileName)
	{
		FILE *errorFile = getErrorFILE();
		result = error_insideVa(nullptr, errorFile, msg_format, args);
		va_end(args);
		fflush(errorFile);
	}
	else
	{
		FILE *logFile = getLogFILE();
		FILE *errorFile = getErrorFILE();
		result = error_insideVa(logFile, errorFile, msg_format, args);
		va_end(args);
		fflush(logFile);
		fflush(errorFile);
	}
	if (result < 0)
	{
		throw AkaneSystemException("Error: Writing %s to %s, %s failed.", msg_format, logFileName, errorFileName);
	}
#endif
}

void AkaneUtils::Logger::active(int destState)
{
	if (nowState == destState)
		return;

	if (nowState == -1)
		closeAll();
	else if (logFileName == errorFileName && (nowState & 0b10) != (destState & 0b10))
	{
		if (nowState & 0b10)
		{
			closeError();
		}
		else
		{
			closeLog();
		}
	}

	switch (destState)
	{
	case writingLogStream:
		if (!openedLogStream)
			openedLogStream = new ofstream(logFileName, ios::app);
		break;
	case writingErrorStream:
		if (!openedErrorStream)
			openedErrorStream = new ofstream(errorFileName, ios::app);
		break;
	case writingLogFILE:
		if (!openedLogFILE)
			openedLogFILE = fopen(logFileName.c_str(), "a");
		break;
	case writingErrorFILE:
		if (!openedErrorFILE)
			openedErrorFILE = fopen(errorFileName.c_str(), "a");
		break;
	}

	nowState = destState;
}

void AkaneUtils::Logger::closeLog()
{
	if (openedLogFILE)
	{
		fclose(openedLogFILE);
		openedLogFILE = nullptr;
	}
	if (openedErrorFILE)
	{
		fclose(openedErrorFILE);
		openedErrorFILE = nullptr;
	}
}
void AkaneUtils::Logger::closeError(){
	if (openedLogStream && openedLogStream->is_open())
	{
		openedLogStream->close();
		delete openedLogStream;
		openedLogStream = nullptr;
	}
	if (openedErrorStream && openedErrorStream->is_open())
	{
		openedErrorStream->close();
		delete openedErrorStream;
		openedErrorStream = nullptr;
	}
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

AkaneUtils::Logger::~Logger()
{
	closeAll();
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