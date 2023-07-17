#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	NONE UMETA(DisplayName = "Init state"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),

	MAX UMETA(DisplayName = "MAX"),
};

UCLASS()
class UE_NETWORKDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(VisibleAnywhere)
		class USkeletalMeshComponent*		_mesh = nullptr;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent*				_sphereCollision = nullptr;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_WeaponState)
		EWeaponState						_weaponState = EWeaponState::Dropped;
	UPROPERTY(EditAnywhere)
		class UAnimationAsset*				_fireAnimation = nullptr;

	UFUNCTION()
	void OnRep_WeaponState();
public:	
	void SetState(EWeaponState weaponState);
	inline class USkeletalMeshComponent* GetMesh() { return _mesh; }
	virtual void Fire(const FVector& targetLocation);
};
