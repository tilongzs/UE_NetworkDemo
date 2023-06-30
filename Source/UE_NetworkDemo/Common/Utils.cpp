#include "Utils.h"
#include <chrono>

using namespace std;
using namespace chrono;

void Log(const FString& log, float time, FColor color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, time, color, GetCurrentTimeStr(true) +  TEXT(" ") + log);
	}	
}

void LogWarning(const FString& log, float time /*= 10.f*/)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, time, FColor::Yellow, GetCurrentTimeStr(true) + TEXT(" ") + log);
	}
}

void LogError(const FString& log, float time /*= 15.f*/)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, time, FColor::Red, GetCurrentTimeStr(true) + TEXT(" ") + log);
	}
}

FString GetCurrentTimeStr(bool hasMillisecond)
{
	if (hasMillisecond)
	{
		uint64_t timestamp(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()); // 获取时间戳（毫秒）

		uint64_t milli = timestamp + 8 * 60 * 60 * 1000; // 转为东八区北京时间
		auto mTime = milliseconds(milli);
		auto tp = time_point<system_clock, milliseconds>(mTime);
		auto tt = system_clock::to_time_t(tp);
		tm now;
		gmtime_s(&now, &tt);
		char str[32] = { 0 };
		sprintf_s(str, "%02d-%02d-%02d %02d:%02d:%02d.%03d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, int(timestamp % 1000));
		return str;
	}
	else
	{
		time_t now = time(nullptr);
		char timeBuf[20] = { 0 };
		std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
		return timeBuf;
	}
}
