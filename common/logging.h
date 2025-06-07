#pragma once
#include "stdio.h"
#include "stdarg.h"
#include "limits.h"

// ログレベル
#ifndef LOGLEVEL
#define	LOGLEVEL	LOGLEVEL_INFO
#endif

// デバッグマクロ
#ifdef _DEBUG
#define LOGOUT_INIT(level, path)	logging.init(level, path)
#define LOGOUT(level, format, ...)	logging.logout(level, format, __VA_ARGS__)
#define LOGOUT_FLUSH()				logging.flush()
#define	LOGOUT_END()				logging.end()
#else
#define LOGOUT_INIT(level, path)
#define LOGOUT(level, format, ...)
#define LOGOUT_FLUSH()
#define	LOGOUT_END()
#endif

enum LogLevel {
	LOGLEVEL_CRITICAL = 0,
	LOGLEVEL_ERROR,
	LOGLEVEL_WARNING,
	LOGLEVEL_INFO,
	LOGLEVEL_DEBUG,
	LOGLEVEL_TRACE,
	LOGLEVEL_ALL
};

class Logging {
public:
	int init(int level, const char* logPath);
	int logprintf(int _level, const char* format, ...);
	int logprintf(const char* format, ...);
	int logout(int _level, const char *format, ...);
	int logout(const char* format, ...);
	int flush();
	int end();
private:
	bool initialized = false;
	FILE* f;
	int level = INT_MAX;

	int logprintf(int _level, const char* _format, va_list _args);
	int logout(int _level, const char* format, va_list _args);
};