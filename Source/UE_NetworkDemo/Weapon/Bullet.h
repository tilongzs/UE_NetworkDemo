#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class UE_NETWORKDEMO_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent*		_mesh;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent*			_sphereCollision;
	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent*	_projectileMovementComponent = nullptr;
	UPROPERTY(EditAnywhere)
		class UParticleSystem*			_tracerPartical;
	UPROPERTY(EditAnywhere)
		class UParticleSystem*			_destroyPartical;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
		TSubclassOf<UDamageType>		_DamageType;
	class UParticleSystemComponent*		_particleSystemComponent = nullptr;
	
	float								_damage = 40;

	UFUNCTION() 
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Server, Reliable)
		void Server_RewindHit(FVector hitLocation, double timeSeconds, class ABlasterCharacter* character);
public:	
	virtual void Tick(float DeltaTime) override;

};
