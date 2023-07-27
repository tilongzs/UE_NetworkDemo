#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 实弹武器
 */
UCLASS()
class UE_NETWORKDEMO_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	AProjectileWeapon();
protected:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ABullet> _BulletClass;

public:
	virtual bool Fire(const FVector& targetLocation, bool isUpdate, int32 ammo) override;
};
