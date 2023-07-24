#include "MyPlayerState.h"
#include "Common/Utils.h"
#include "Net/UnrealNetwork.h"

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, _killCount);
	DOREPLIFETIME(AMyPlayerState, _deathCount);
}

void AMyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

 	//auto uniqueID = GetUniqueID();
 	//Log(FString::Printf(TEXT("AMyPlayerState::OnRep_PlayerName uniqueID:%u PlayerName:%s"), uniqueID, *GetPlayerName()));
	_dlgOnRepPlayerName.Broadcast(GetPlayerName());
}

void AMyPlayerState::OnRep_KillCount()
{
	_dlgKillCountChanged.Broadcast(_killCount);
}

void AMyPlayerState::OnRep_DeathCount()
{
	_dlgDeathCountChanged.Broadcast(_deathCount);
}

void AMyPlayerState::Server_OnDeath()
{
	_deathCount++;

	OnRep_DeathCount();
}

void AMyPlayerState::Server_OnKillSomebody()
{
	_killCount++;

	OnRep_KillCount();
}
