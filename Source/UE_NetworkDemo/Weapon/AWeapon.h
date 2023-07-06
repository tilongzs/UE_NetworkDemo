#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	NONE UMETA(DisplayName = "Init state"),
	Equipped UMETA(DisplayName = "Equipped"),

	MAX UMETA(DisplayName = "MAX"),
};

UCLASS()
class UE_NETWORKDEMO_API AAWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AAWeapon();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		class USkeletalMeshComponent*		_mesh;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent*				_sphereCollision;
	UPROPERTY(VisibleAnywhere)
		EWeaponState						_weaponState;

	UFUNCTION() 
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	

};
