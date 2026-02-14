#include "logging.h"
#include "stdarg.h"
#include "time.h"

int Logging::init(int _level, const char* _logPath, bool _flushing, bool _append, bool _bFileDelCheck)
{
	int ret;
	//if (fopen_s(&f, _logPath, "w") != 0) {
	//	return -1;
	//}

	// Parameter Set
	level = _level;
	flushing = _flushing;
	append = _append;
	bFileDelCheck = _bFileDelCheck;

	ret = MultiByteToWideChar(CP_ACP, 0, _logPath, -1, logPathW, _countof(logPathW));
	if (ret == 0) return -1;

	// Open the file
	ret = fileOpen();

	if (ret < 0) {
		return -2;
	}

	initialized = true;

	return 0;
}

int Logging::fileOpen()
{
	if (append == false) {
		handle = CreateFile(
			logPathW,
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_DELETE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}
	else {
		handle = CreateFile(
			logPathW,
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_DELETE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}

	if (handle == INVALID_HANDLE_VALUE) return -1;
	else return 0;
}

int Logging::logprintf(int _level, const char *_format, va_list _args)
{
	if (initialized == false) return -1;
	if (_level > level) return 1;

	// ファイル名変更･削除チェック
	if (bFileDelCheck == true) {
		WIN32_FILE_ATTRIBUTE_DATA fileInfo;
		if (!GetFileAttributesExW(logPathW, GetFileExInfoStandard, &fileInfo)) {
			// ファイルが存在しない（削除された）
			fileClose();
			
			int ret = fileOpen();
			if (ret < 0) {
				initialized = false;
				return -1;
			}
		}
	}

	// ログ出力
	//vfprintf(f, _format, _args);
	char str[4096];
	vsprintf_s(str, sizeof(str), _format, _args);
	
	DWORD bytesWritten;
	SetFilePointer(handle, 0, NULL, FILE_END);
	if (WriteFile(handle, str, strlen(str), &bytesWritten, NULL) == false) return -2;

	// flush
	if (flushing == true) flush();

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

	//fprintf(f, "[%04d/%02d/%02d %02d:%02d:%02d] ",
	//	timeObject.tm_year + 1900,
	//	timeObject.tm_mon + 1,
	//	timeObject.tm_mday,
	//	timeObject.tm_hour,
	//	timeObject.tm_min,
	//	timeObject.tm_sec
	//);

	int ret;
	ret = logprintf(_level, "[%04d/%02d/%02d %02d:%02d:%02d] ",
			timeObject.tm_year + 1900,
			timeObject.tm_mon + 1,
			timeObject.tm_mday,
			timeObject.tm_hour,
			timeObject.tm_min,
			timeObject.tm_sec
		);
	if (ret < 0) return ret;

	ret = logprintf(_level, format, _args);
	if (ret < 0) return ret;

	// 改行出力
	//fprintf(f, "\n");
	ret = logprintf(_level, "\n");
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
	//fflush(f);
	FlushFileBuffers(handle);

	return 0;
}

int Logging::end()
{
	//fclose(f);
	CloseHandle(handle);

	initialized = false;

	return 0;
}