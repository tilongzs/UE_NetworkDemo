﻿#pragma once

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
		class USkeletalMeshComponent*		_mesh;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent*				_sphereCollision;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_WeaponState)
		EWeaponState						_weaponState;

	UFUNCTION()
	void OnRep_WeaponState();
public:	
	void SetState(EWeaponState weaponState);
	inline class USkeletalMeshComponent* GetMesh() { return _mesh; }
};
