#include "logging.h"
#include "stdarg.h"
#include "time.h"

int Logging::init(int _level, const char* _logPath)
{
	if (fopen_s(&f, _logPath, "w") != 0) {
		return -1;
	}

	initialized = true;
	level = _level;

	return 0;
}

int Logging::logprintf(int _level, const char *_format, va_list _args)
{
	if (initialized == false) return -1;
	if (_level > level) return 1;

	// ログ出力
	vfprintf(f, _format, _args);

	return 0;
}

int Logging::logprintf(int _level, const char* format, ...)
{
	if (initialized == false) return -1;
	if (_level > level) return 1;

	va_list args;
	va_start(args, format);

	int ret;
	ret = logprintf(_level, format, args);

	va_end(args);

	return ret;
}

int Logging::logprintf(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	int ret;
	ret = logprintf(LOGLEVEL_TRACE, format, args);

	va_end(args);

	return ret;
}

int Logging::logout(int _level, const char* format, va_list _args)
{
	if (initialized == false) return -1;
	if (_level > level) return 1;

	// 時刻の追加
	time_t timeValue;
	struct tm timeObject;

	time(&timeValue);
	localtime_s(&timeObject, &timeValue);

	fprintf(f, "[%04d/%02d/%02d %02d:%02d:%02d] ",
		timeObject.tm_year + 1900,
		timeObject.tm_mon + 1,
		timeObject.tm_mday,
		timeObject.tm_hour,
		timeObject.tm_min,
		timeObject.tm_sec
	);

	int ret;
	ret = logprintf(_level, format, _args);

	// 改行出力
	fprintf(f, "\n");

	return ret;
}

int Logging::logout(int _level, const char* format, ...)
{
	if (initialized == false) return -1;
	if (_level > level) return 1;

	va_list args;
	va_start(args, format);

	int ret;
	ret = logout(_level, format, args);

	va_end(args);

	return ret;
}

int Logging::logout(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	int ret;
	ret = logout(LOGLEVEL_TRACE, format, args);

	va_end(args);

	return ret;
}

int Logging::flush()
{
	fflush(f);

	return 0;
}

int Logging::end()
{
	fclose(f);
	initialized = false;

	return 0;
}