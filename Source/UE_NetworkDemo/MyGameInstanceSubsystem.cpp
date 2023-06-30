// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "Common/Utils.h"

void UMyGameInstanceSubsystem::ServerTravelLobby()
{
	UWorld* world = GetWorld();
	if (world)
	{
		world->ServerTravel("/Game/Maps/Lobby?listen");
		Log("OpenLobby sucess");
	}
	else
	{
		Log("OpenLobby failed");
	}
}

void UMyGameInstanceSubsystem::CallOpenLevel(const FString& address)
{
	UGameplayStatics::OpenLevel(this, *address);
}

void UMyGameInstanceSubsystem::CallClientTravel(const FString& address)
{
	APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (playerController)
	{
		playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		Log("CallClientTravel failed");
	}
}

void UMyGameInstanceSubsystem::InitOnlineSession()
{
	if (_onlineSession)
	{
		// 在线子系统已经初始化
		return;
	}

	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem)
	{
		_onlineSession = onlineSubsystem->GetSessionInterface();
		if (GEngine)
		{
			Log(FString::Printf(TEXT("当前网络子系统为:%s"), *onlineSubsystem->GetSubsystemName().ToString()));
		}
	}
	else
	{
		Log(TEXT("获取在线子系统失败"));
	}
}

void UMyGameInstanceSubsystem::CreateGameSession()
{
	if (!_onlineSession.IsValid())
	{
		Log(TEXT("在线子系统未初始化"));
		return;
	}

	Log(TEXT("开始创建游戏会话"));

	// 检查游戏会话是否已存在
	auto* namedSession = _onlineSession->GetNamedSession(NAME_GameSession);
	if (namedSession != nullptr)
	{
		_onlineSession->DestroySession(NAME_GameSession);
	}

	// 创建游戏会话
	_dhOnCreateSessionComplete = _onlineSession->AddOnCreateSessionCompleteDelegate_Handle(_dlgOnCreateSessionComplete);
	TSharedPtr<FOnlineSessionSettings> sessionSetting = MakeShared<FOnlineSessionSettings>();
	sessionSetting->bIsLANMatch = true; // 使用局域网，方便本地测试
	sessionSetting->NumPublicConnections = 4;
	sessionSetting->bAllowJoinInProgress = true;
	sessionSetting->bAllowJoinViaPresence = true;
	sessionSetting->bShouldAdvertise = true;
	sessionSetting->bUsesPresence = true;
	sessionSetting->bUseLobbiesIfAvailable = true;
	sessionSetting->BuildUniqueId = 1; // 生成唯一会话ID，以保证其他用户能搜索到
	sessionSetting->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // 自定义参数
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!_onlineSession->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *sessionSetting))
	{
		_onlineSession->ClearOnCreateSessionCompleteDelegate_Handle(_dhOnCreateSessionComplete);
		LogError(TEXT("执行创建游戏会话失败"));
		return;
	}
}

void UMyGameInstanceSubsystem::FindAndJoinGameSession()
{
	if (!_onlineSession.IsValid())
	{
		return;
	}

	Log(TEXT("开始查找游戏会话"));

	// 查找游戏会话
	_dhOnFindSessionsComplete = _onlineSession->AddOnFindSessionsCompleteDelegate_Handle(_dlgOnFindSessionsComplete);
	_onlineSessionSearch = MakeShared<FOnlineSessionSearch>();
	_onlineSessionSearch->MaxSearchResults = 100;
	_onlineSessionSearch->bIsLanQuery = true; // 使用局域网，方便本地测试
	_onlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!_onlineSession->FindSessions(*localPlayer->GetPreferredUniqueNetId(), _onlineSessionSearch.ToSharedRef()))
	{
		_onlineSession->ClearOnFindSessionsCompleteDelegate_Handle(_dhOnFindSessionsComplete);
		LogError(TEXT("执行查找游戏会话失败"));
	}
}

void UMyGameInstanceSubsystem::DestroyCurrentSession()
{
	if (!_onlineSession.IsValid())
	{
		return;
	}

	if (_currentSessionName == "")
	{
		Log(TEXT("当前没有游戏会话"));
		return;
	}

	Log(TEXT("开始销毁当前游戏会话"));	
	if (!_onlineSession->DestroySession(_currentSessionName))
	{
		_onlineSession->ClearOnDestroySessionCompleteDelegate_Handle(_dhOnDestroySessionComplete);
		LogError(TEXT("执行销毁游戏会话失败"));
	}
}

void UMyGameInstanceSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	_onlineSession->ClearOnCreateSessionCompleteDelegate_Handle(_dhOnCreateSessionComplete);

	if (bWasSuccessful)
	{
		_currentSessionName = SessionName;
		Log(FString::Printf(TEXT("创建游戏会话%s成功"), *SessionName.ToString()));

		_dhOnSessionParticipantJoined = _onlineSession->AddOnSessionParticipantJoinedDelegate_Handle(_dlgOnSessionParticipantJoined);
		_dhOnSessionParticipantLeft = _onlineSession->AddOnSessionParticipantLeftDelegate_Handle(_dlgOnSessionParticipantLeft);
		_dhOnDestroySessionComplete = _onlineSession->AddOnDestroySessionCompleteDelegate_Handle(_dlgOnDestroySessionComplete);

		// 跳转至游戏大厅地图
		UWorld* world = GetWorld();
		if (world)
		{
			if (!world->ServerTravel("/Game/Maps/Lobby?listen"))
			{
				LogError(TEXT("跳转至游戏大厅地图失败"));
			}
		}
		else
		{
			LogError(TEXT("跳转至游戏大厅地图 获取world失败"));
		}
	}
	else
	{
		_currentSessionName = "";
		LogWarning(TEXT("创建游戏会话失败"));
	}
}

void UMyGameInstanceSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!_onlineSession.IsValid())
	{
		return;
	}
	_onlineSession->ClearOnFindSessionsCompleteDelegate_Handle(_dhOnFindSessionsComplete);

	if (bWasSuccessful)
	{
		Log(FString::Printf(TEXT("查找到%d个游戏会话"), _onlineSessionSearch->SearchResults.Num()));

		for (auto& result : _onlineSessionSearch->SearchResults)
		{
			FString sessionID = result.GetSessionIdStr();
			Log(FString::Printf(TEXT("查找到游戏会话 sessionID:%s userName:%s"), *sessionID, *result.Session.OwningUserName));

			// 可检查MatchType是否一致
			FString matchType;
			result.Session.SessionSettings.Get("MatchType", matchType);
			if (matchType == "FreeForAll")
			{
				Log(FString::Printf(TEXT("--游戏类型:%s"), *matchType));

				// 加入游戏会话
				_dhOnJoinSessionComplete = _onlineSession->AddOnJoinSessionCompleteDelegate_Handle(_dlgOnJoinSessionComplete);
				const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
				if (!_onlineSession->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, result))
				{
					_onlineSession->ClearOnJoinSessionCompleteDelegate_Handle(_dhOnJoinSessionComplete);
					LogError(TEXT("执行加入游戏会话失败"));
				}
				break;
			}
		}
	}
	else
	{
		Log(TEXT("查找游戏会话失败"));
	}
}

void UMyGameInstanceSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (!_onlineSession.IsValid())
	{
		return;
	}
	_currentSessionName = SessionName;
	_onlineSession->ClearOnJoinSessionCompleteDelegate_Handle(_dhOnJoinSessionComplete);

	if (result == EOnJoinSessionCompleteResult::Success)
	{
		Log(TEXT("加入游戏会话成功"));

		_dhOnSessionParticipantJoined = _onlineSession->AddOnSessionParticipantJoinedDelegate_Handle(_dlgOnSessionParticipantJoined);
		_dhOnSessionParticipantLeft = _onlineSession->AddOnSessionParticipantLeftDelegate_Handle(_dlgOnSessionParticipantLeft);
		_dhOnDestroySessionComplete = _onlineSession->AddOnDestroySessionCompleteDelegate_Handle(_dlgOnDestroySessionComplete);

		// 获取该会话的连接信息
		FString connectInfo;
		if (_onlineSession->GetResolvedConnectString(NAME_GameSession, connectInfo))
		{
			Log(FString::Printf(TEXT("游戏会话连接信息: %s"), *connectInfo));

			// 连接服务端
			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(connectInfo, ETravelType::TRAVEL_Absolute);
			}
			else
			{
				LogWarning(TEXT("跳转至大厅地图失败"));
			}
		}
		else
		{
			LogWarning(TEXT("获取游戏会话的连接信息失败"));
		}
	}
	else if (result == EOnJoinSessionCompleteResult::AlreadyInSession)
	{
		LogWarning(TEXT("已经在游戏会话中"));
	}
	else
	{
		LogWarning(TEXT("加入游戏会话失败"));
	}
}

void UMyGameInstanceSubsystem::OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UniqueId)
{
	if (!_onlineSession.IsValid())
	{
		return;
	}
	_onlineSession->ClearOnSessionParticipantJoinedDelegate_Handle(_dhOnSessionParticipantJoined);

	Log(TEXT("新玩家加入会话"));
}

void UMyGameInstanceSubsystem::OnSessionParticipantLeft(FName SessionName, const FUniqueNetId& UniqueId, EOnSessionParticipantLeftReason LeaveReason)
{
	if (!_onlineSession.IsValid())
	{
		return;
	}
	_onlineSession->ClearOnSessionParticipantLeftDelegate_Handle(_dhOnSessionParticipantLeft);

	Log(FString::Printf(TEXT("玩家离开会话 reason:%d"), LeaveReason));
}

void UMyGameInstanceSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!_onlineSession.IsValid())
	{
		return;
	}
	_onlineSession->ClearOnDestroySessionCompleteDelegate_Handle(_dhOnDestroySessionComplete);

	Log(FString::Printf(TEXT("会话已销毁 %d"), bWasSuccessful));
}
