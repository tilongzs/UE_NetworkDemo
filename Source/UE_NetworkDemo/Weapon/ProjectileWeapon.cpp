#include "ProjectileWeapon.h"
#include "Bullet.h"
#include "../Common/Utils.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

AProjectileWeapon::AProjectileWeapon()
{
	
}

void AProjectileWeapon::Fire(const FVector& targetLocation)
{
	Super::Fire(targetLocation);

	if (HasAuthority())
	{
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
	}
}
