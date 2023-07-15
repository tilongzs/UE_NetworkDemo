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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent*		_mesh;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent*			_sphereCollision;
public:	
	virtual void Tick(float DeltaTime) override;

};
