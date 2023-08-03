#include "Bullet.h"
#include "../Common/Utils.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "../Character/BlasterCharacter.h"
#include "../DamageComponent.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	_sphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	_sphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	
	SetRootComponent(_sphereCollision);

	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_mesh->SetupAttachment(RootComponent);

	_projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	_projectileMovementComponent->bRotationFollowsVelocity = true;
	_projectileMovementComponent->SetIsReplicated(true);
}

void ABullet::Destroyed()
{
	Super::Destroyed();

	if (_destroyPartical)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _destroyPartical, GetActorTransform());
	}
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator())
	{
		if (GetInstigator()->IsLocallyControlled())
		{
			// 是否击中由发射端来进行前期判断
			_sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnComponentBeginOverlap);
		}
	}
	
	if (_tracerPartical)
	{
		_particleSystemComponent = UGameplayStatics::SpawnEmitterAttached(_tracerPartical, _mesh, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
		if (_particleSystemComponent)
		{
			_particleSystemComponent->SetupAttachment(_mesh);
		}
		else
		{
			LogError("SpawnEmitterAttached failed");
		}
	}
}

void ABullet::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(OtherActor);
	ABlasterCharacter* instigator = Cast<ABlasterCharacter>(GetInstigator());
	if (character && instigator && OtherActor != instigator)
	{
		// instigator->IsLocallyControlled()
		if (HasAuthority())
		{
			UGameplayStatics::ApplyPointDamage(character, _damage, FVector::ZeroVector, SweepResult, instigator->GetController(), this, _DamageType);
			Destroy();
		}
		else if (character->IsUseServerRewind())
		{
			// 通知服务端验证碰撞
			ABlasterCharacter* playerCharacter = Cast<ABlasterCharacter>(instigator);
			if (playerCharacter)
			{
				Log("Call Server_RewindHit");
				Server_RewindHit(SweepResult.ImpactPoint, GetWorld()->GetTimeSeconds() + playerCharacter->GetDifferFromServerTime(), character);
			}
			_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABullet::Server_RewindHit_Implementation(FVector hitLocation, double timeSeconds, class ABlasterCharacter* character)
{
	if (!character)
	{
		LogError("ABullet::Server_RewindHit_Implementation character is NULL");
		return;
	}

	Log("Server_RewindHit begin");

	UDamageComponent* damageComp = character->GetDamageComp();
	if (damageComp)
	{
		//DrawDebugSphere(GetWorld(), hitLocation, 12, 12, FColor::Red, true);

		// 回溯验证是否击中
		_projectileMovementComponent->StopMovementImmediately();
		_projectileMovementComponent->StopSimulating(FHitResult());
		SetActorLocation(hitLocation);
		if (damageComp->RewindHitConfirm(timeSeconds))
		{
			UGameplayStatics::ApplyPointDamage(character, _damage, FVector(), FHitResult(), GetInstigator()->GetController(), this, _DamageType);

			_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		Destroy();
	}
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

