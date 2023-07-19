#include "LobbyGameMode.h"
#include "Common/Utils.h"
#include "GameFrameWork/GameStateBase.h"
#include "GameFrameWork/PlayerState.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Math/UnrealMathUtility.h"

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

void ALobbyGameMode::OnCharacterKilled(ACharacter* character)
{
    APlayerController* playerController = Cast<APlayerController>(character->GetController());
    if (playerController)
    {
        playerController->DisableInput(playerController);
    }
    character->Reset();    
    character->Destroy();

    // 重建角色
   // GetWorldTimerManager().SetTimer(_timerRespawnCharacter, this, &ALobbyGameMode::OnTimerRespawnCharacter, 2);
    FTimerHandle timerRespawnCharacter;
    GetWorldTimerManager().SetTimer(timerRespawnCharacter, [&, playerController] {
        TArray<AActor*> playerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);
        int randomIndex = FMath::RandRange(0, playerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(playerController, playerStarts[randomIndex]);
        }, 2, false);
}
