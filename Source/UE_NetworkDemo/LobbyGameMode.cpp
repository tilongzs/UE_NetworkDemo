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

            if (playersNum >= 4)
            {
                // 跳转至关卡1
                bUseSeamlessTravel = true;
                auto world = GetWorld();
                if (world)
                {
                    Log(TEXT("人数足够，开始跳转至关卡1"));
                    if (!world->ServerTravel("/Game/Maps/Level1", true))
                    {
                        LogError(TEXT("跳转至关卡1失败"));
                    }
                }
            }
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
