#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class UE_NETWORKDEMO_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateAddPlayerState, APlayerState*, PlayerState);
	UPROPERTY(BlueprintAssignable)
		FDelegateAddPlayerState	_dlgAddPlayerState;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateRemovePlayerState, APlayerState*, PlayerState);
	UPROPERTY(BlueprintAssignable)
		FDelegateRemovePlayerState	_dlgRemovePlayerState;
};
