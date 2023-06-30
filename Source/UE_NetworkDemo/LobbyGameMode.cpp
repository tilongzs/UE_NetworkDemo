#include "LobbyGameMode.h"
#include "Common/Utils.h"
#include "GameFrameWork/GameStateBase.h"
#include "GameFrameWork/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

    if (GameState)
    {
        APlayerState* playerState = NewPlayer->GetPlayerState<APlayerState>();
        if (playerState)
        {
            FString playerName = playerState->GetPlayerName();
            int32 playersNum = GameState.Get()->PlayerArray.Num();
            Log(FString::Printf(TEXT("%s 加入游戏!当前玩家数量:%d"), *playerName, playersNum));
        }
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    if (GameState)
    {
        APlayerState* playerState = Exiting->GetPlayerState<APlayerState>();
        if (playerState)
        {
            FString playerName = playerState->GetPlayerName();
            int32 playersNum = GameState.Get()->PlayerArray.Num();
            Log(FString::Printf(TEXT("%s 离开游戏!当前玩家数量:%d"), *playerName, playersNum - 1));
        }
    }
}
