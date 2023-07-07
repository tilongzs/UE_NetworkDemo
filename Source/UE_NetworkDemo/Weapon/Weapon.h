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
	virtual void OnRep_AttachmentReplication() override;

	UPROPERTY(VisibleAnywhere)
		class USkeletalMeshComponent*		_mesh;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent*				_sphereCollision;
	UPROPERTY(VisibleAnywhere)
		EWeaponState						_weaponState;

public:	
	void SetState(EWeaponState weaponState);
};
