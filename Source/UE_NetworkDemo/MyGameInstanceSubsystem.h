// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "MyGameInstanceSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class UE_NETWORKDEMO_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ServerTravelLobby();

	UFUNCTION(BlueprintCallable)
	void CallOpenLevel(const FString& address);

	UFUNCTION(BlueprintCallable)
	void CallClientTravel(const FString& address);

	UFUNCTION(BlueprintCallable)
	void InitOnlineSession();

	UFUNCTION(BlueprintCallable)
	void CreateGameSession();

	UFUNCTION(BlueprintCallable)
	void FindAndJoinGameSession();

protected:
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe>/*IOnlineSessionPtr*/	_onlineSession;
	TSharedPtr<FOnlineSessionSearch> _onlineSessionSearch;

	FOnCreateSessionCompleteDelegate	_dlgOnCreateSessionComplete = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	FOnFindSessionsCompleteDelegate		_dlgOnFindSessionsComplete = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete);
	FOnJoinSessionCompleteDelegate		_dlgOnJoinSessionComplete = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete);

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type result);
};
