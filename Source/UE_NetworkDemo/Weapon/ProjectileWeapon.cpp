#include "ProjectileWeapon.h"
#include "Bullet.h"
#include "../Common/Utils.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

AProjectileWeapon::AProjectileWeapon()
{
	_ammo = 21;
}

bool AProjectileWeapon::Fire(const FVector& targetLocation, bool isUpdate, int32 ammo)
{
	Super::Fire(targetLocation, isUpdate, ammo);

	if (_ammo <= 0)
	{
		return false;
	}

	if (!HasAuthority())
	{
		return false;
	}

	// 生成子弹
	const USkeletalMeshSocket* muzzleSocket = _mesh->GetSocketByName(FName("MuzzleFlash"));
	UWorld* world = GetWorld();
	if (world && _BulletClass && muzzleSocket)
	{
		FVector muzzleLocation = muzzleSocket->GetSocketLocation(_mesh); // 枪口位置
		FActorSpawnParameters param;
		param.Owner = this;
		param.Instigator = Cast<APawn>(GetOwner());
		FRotator bulletRotator = UKismetMathLibrary::FindLookAtRotation(muzzleLocation, targetLocation);
		ABullet* bullet = world->SpawnActor<ABullet>(_BulletClass, muzzleLocation, bulletRotator, param);
		if (bullet)
		{
			//Log(TEXT("生成子弹成功"));
			return true;
		}
		else
		{
			LogError(TEXT("生成子弹失败"));
		}
	}
	else
	{
		LogError(TEXT("生成子弹条件不符合"));
	}

	if (!isUpdate)
	{
		// 子弹数目修正
		_ammo++;
	}
	
	return false;
}
