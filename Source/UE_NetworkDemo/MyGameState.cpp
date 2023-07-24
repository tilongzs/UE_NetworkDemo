#include "MyGameState.h"
#include "Common/Utils.h"
#include "GameFramework/PlayerState.h"

void AMyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	// GetUniqueID()不为空，GetPlayerName()为空！
	//auto uniqueID = PlayerState->GetUniqueID();
	//Log(FString::Printf(TEXT("AMyGameState::AddPlayerState uniqueID:%u PlayerName:%s"), uniqueID, *PlayerState->GetPlayerName()));

	_dlgAddPlayerState.Broadcast(PlayerState);
}

void AMyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	Log(FString::Printf(TEXT("AMyGameState %s 离开游戏!当前玩家数量:%d"), *PlayerState->GetPlayerName(), PlayerArray.Num()));

	_dlgRemovePlayerState.Broadcast(PlayerState);
}
