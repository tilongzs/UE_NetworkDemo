#include "AWeapon.h"
#include "Components/SphereComponent.h"
#include "../Common/Utils.h"
#include "../Character/BlacterCharacter.h"

AAWeapon::AAWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(_mesh);
	_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	_sphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_sphereCollision->SetupAttachment(RootComponent);
}

void AAWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		_sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AAWeapon::OnSphereBeginOverlap);
	}
}

void AAWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlacterCharacter* player = Cast<ABlacterCharacter>(OtherActor);
	if (HasAuthority() && player)
	{
		SetActorHiddenInGame(true);
		//Destroy();
	}
}

void AAWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

