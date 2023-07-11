#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	auto volocity = _blasterCharacter->GetVelocity();
	//volocity.Z = 0; // 忽略垂直方向速度
	_speed = volocity.Size();

	_isFalling = _blasterCharacter->GetCharacterMovement()->IsFalling();
	_isAccelerating = _blasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;
	_isWeaponEquipped = _blasterCharacter->IsWeaponEquipped();
}
