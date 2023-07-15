#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	_sphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	_sphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetRootComponent(_sphereCollision);

	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_mesh->SetupAttachment(RootComponent);
}

void ABullet::BeginPlay()
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

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

