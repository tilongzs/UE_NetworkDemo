﻿#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	_sphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	_sphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetRootComponent(_sphereCollision);

	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_mesh->SetupAttachment(RootComponent);

	_projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	_projectileMovementComponent->bRotationFollowsVelocity = true;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		_projectileMovementComponent->SetIsReplicated(true);
	}

	if (_tracerPartical)
	{
		_particleSystemComponent = UGameplayStatics::SpawnEmitterAttached(_tracerPartical, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
		_particleSystemComponent->SetupAttachment(RootComponent);
	}

}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

