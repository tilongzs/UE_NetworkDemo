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
	UPROPERTY(EditAnywhere)
		float								_zoomedFOV = 30.f;
	UPROPERTY(EditAnywhere)
		float								_zoomInterpSpeed = 20.f;
	UPROPERTY(EditAnywhere)
		float								_fireDelay = 0.15f; // 射击间隔
	UPROPERTY(VisibleAnywhere)
		int32								_ammo = 0; // 剩余弹药
	int32									_ammoUpdateSequence = 0; // 剩余弹药 同步序列

	UFUNCTION()
	void OnRep_WeaponState();

public:	
	void SetState(EWeaponState weaponState);
	inline class USkeletalMeshComponent* GetMesh() { return _mesh; }
	virtual bool Fire(const FVector& targetLocation, bool isUpdate, int32 ammo);
	FORCEINLINE float GetZoomedFOV() { return _zoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() { return _zoomInterpSpeed; }
	FORCEINLINE float GetFireDelay() { return _fireDelay; }
	FORCEINLINE uint32 GetAmmo() { return _ammo; };
};
