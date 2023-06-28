#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class Utils
{
public:
	
};

void Log(const FString& log, float time = 10.f, FColor color = FColor::Blue);
void LogWarning(const FString& log, float time = 15.f);
void LogError(const FString& log, float time = 20.f);

FString GetCurrentTimeStr(bool hasMillisecond);