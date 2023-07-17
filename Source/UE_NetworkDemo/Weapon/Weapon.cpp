#include "Weapon.h"
#include "../Common/Utils.h"
#include "../Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(_mesh);
	_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);	

	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	_sphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_sphereCollision->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();	

	if (HasAuthority())
	{
 		SetReplicates(true);
 		_mesh->SetIsReplicated(true);
		_mesh->SetSimulatePhysics(true);		
	}
	else
	{
		_mesh->SetSimulatePhysics(false);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, _weaponState);
}

void AWeapon::OnRep_WeaponState()
{
	if (_weaponState == EWeaponState::Equipped)
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AWeapon::SetState(EWeaponState weaponState)
{
	_weaponState = weaponState;
	if (_weaponState == EWeaponState::Equipped)
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		_mesh->SetSimulatePhysics(false);
	}
	else
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		if (HasAuthority())
		{
			_mesh->SetSimulatePhysics(true);
		}
	}
}

void AWeapon::Fire(const FVector& targetLocation)
{
	if (_fireAnimation)
	{
		_mesh->PlayAnimation(_fireAnimation, false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

