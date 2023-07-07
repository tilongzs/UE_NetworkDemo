#include "Weapon.h"
#include "../Common/Utils.h"
#include "../Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
// 	bAlwaysRelevant = true;
// 	bNetLoadOnClient = true;
	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(_mesh);
	_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	_mesh->SetSimulatePhysics(true);
//	_mesh->SetIsReplicated(true);

	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	_sphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_sphereCollision->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		_sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereBeginOverlap);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* player = Cast<ABlasterCharacter>(OtherActor);
	if (HasAuthority() && player)
	{
		//SetActorHiddenInGame(true);
		//Destroy();
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

