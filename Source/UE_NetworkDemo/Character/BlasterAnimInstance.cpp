#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "../Common/Utils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	_blasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!_blasterCharacter)
	{
		return;
	}

	// 获取速度
	FVector velocity = _blasterCharacter->GetVelocity();
	//volocity.Z = 0; // 忽略垂直方向速度
	_speed = velocity.Size();

	_isFalling = _blasterCharacter->GetCharacterMovement()->IsFalling();
	_isAccelerating = _blasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;
	_isWeaponEquipped = _blasterCharacter->IsWeaponEquipped();
	_isCrouched = _blasterCharacter->bIsCrouched;
	_isAiming = _blasterCharacter->IsAiming();

	FRotator aimRotation = _blasterCharacter->GetBaseAimRotation();
	FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(velocity);
	FRotator faceRotation = _blasterCharacter->GetActorRotation();
	FRotator deltaMovementToAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	_deltaMovementToAimRotation = FMath::RInterpTo(_deltaMovementToAimRotation, deltaMovementToAimRotation, DeltaSeconds, 15.f);
	_yawOffset = _deltaMovementToAimRotation.Yaw;
	//Log(FString::Printf(TEXT("_yawOffset:%.1lf"), _yawOffset));
	//Log(FString::Printf(TEXT("aimYaw:%.1lf velocityYaw:%.1lf"), aimRotation.Yaw, movementRotation.Yaw));
}
