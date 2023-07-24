#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UE_NETWORKDEMO_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_PlayerName() override;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_KillCount)
		int32		_killCount = 0;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_DeathCount)
		int32		_deathCount = 0;

	UFUNCTION()
		void OnRep_KillCount();
	UFUNCTION()
		void OnRep_DeathCount();

public:
	void Server_OnDeath();
	void Server_OnKillSomebody();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateKillCountChanged, int32, killCount);
	UPROPERTY(BlueprintAssignable)
		FDelegateKillCountChanged	_dlgKillCountChanged;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateDeathCountChanged, int32, deathCount);
	UPROPERTY(BlueprintAssignable)
		FDelegateDeathCountChanged	_dlgDeathCountChanged;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateOnRepPlayerName, FString, playerName);
	UPROPERTY(BlueprintAssignable)
		FDelegateOnRepPlayerName	_dlgOnRepPlayerName;

	UFUNCTION(BlueprintCallable)
		FORCEINLINE int32 GetKillCount(){return _killCount;}
	UFUNCTION(BlueprintCallable)
		FORCEINLINE int32 GetDeathCount(){return _deathCount;}
};
