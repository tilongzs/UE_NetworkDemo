#include "Weapon.h"
#include "../Common/Utils.h"
#include "../Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"

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
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		_mesh->SetSimulatePhysics(false);
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::SetState(EWeaponState weaponState)
{
	if (weaponState == EWeaponState::Equipped)
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		_mesh->SetSimulatePhysics(false);
	}
	else
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		_mesh->SetSimulatePhysics(true);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

