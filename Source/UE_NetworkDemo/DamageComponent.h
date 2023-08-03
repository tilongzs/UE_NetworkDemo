#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageComponent.generated.h"



/// <summary>
/// damageBoxes的状态
/// </summary>
struct DamageBoxState
{
	FVector	location;
	FRotator rotation;
	FVector boxExtent;
};

/// <summary>
/// damageBoxes的历史状态
/// </summary>
struct DamageBoxHistory
{
	double	timeSeconds;
	TMap<FName, TSharedPtr<DamageBoxState>> damageBoxStates;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_NETWORKDEMO_API UDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageComponent();

protected:
	virtual void BeginPlay() override;

	ACharacter*		_character;

	UPROPERTY(EditAnywhere)
		TMap<FName, class UBoxComponent*>	_damageBoxes;
	TDoubleLinkedList<TSharedPtr<DamageBoxHistory>>	_damageBoxCaches;
	double				_cacheTime = 1.5; // 缓存时长（秒）

	void CacheBoxState(); // 缓存_damageBoxes的状态
	void DebugDrawDamageBoxState(const TMap<FName, TSharedPtr<DamageBoxState>>& damageBoxStates, const FColor& color);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetCharacter(class ACharacter* character);
	bool RewindHitConfirm(double timeSeconds);
};
