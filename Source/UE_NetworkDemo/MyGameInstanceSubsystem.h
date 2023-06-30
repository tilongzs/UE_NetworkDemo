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

	UFUNCTION(BlueprintCallable)
	void DestroyCurrentSession();

protected:
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe>/*IOnlineSessionPtr*/	_onlineSession;
	TSharedPtr<FOnlineSessionSearch> _onlineSessionSearch;
	FName _currentSessionName;

	FOnCreateSessionCompleteDelegate	_dlgOnCreateSessionComplete = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	FDelegateHandle						_dhOnCreateSessionComplete;
	FOnFindSessionsCompleteDelegate		_dlgOnFindSessionsComplete = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete);
	FDelegateHandle						_dhOnFindSessionsComplete;
	FOnJoinSessionCompleteDelegate		_dlgOnJoinSessionComplete = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete);
	FDelegateHandle						_dhOnJoinSessionComplete;
	FOnSessionParticipantJoinedDelegate _dlgOnSessionParticipantJoined = FOnSessionParticipantJoinedDelegate::CreateUObject(this, &ThisClass::OnSessionParticipantJoined);
	FDelegateHandle						_dhOnSessionParticipantJoined;
	FOnSessionParticipantLeftDelegate	_dlgOnSessionParticipantLeft = FOnSessionParticipantLeftDelegate::CreateUObject(this, &ThisClass::OnSessionParticipantLeft);
	FDelegateHandle						_dhOnSessionParticipantLeft;
	FOnDestroySessionCompleteDelegate	_dlgOnDestroySessionComplete = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete);
	FDelegateHandle						_dhOnDestroySessionComplete;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type result);
	void OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UniqueId);
	void OnSessionParticipantLeft(FName SessionName, const FUniqueNetId& UniqueId, EOnSessionParticipantLeftReason LeaveReason);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
};
