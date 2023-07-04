#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

UCLASS()
class UE_NETWORKDEMO_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
		class ABlacterCharacter*	_blasterCharacter = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		float	_speed = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		bool	_isFalling = false;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		bool	_isAccelerating = false;
};
