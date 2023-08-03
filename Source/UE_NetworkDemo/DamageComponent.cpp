#include "DamageComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Common/Utils.h"
#include "Weapon/Bullet.h"

UDamageComponent::UDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	_damageBoxes.Add(TEXT("spine_01"), CreateDefaultSubobject<UBoxComponent>(TEXT("spine_01")));
	_damageBoxes.Add(TEXT("head"), CreateDefaultSubobject<UBoxComponent>(TEXT("head")));
	_damageBoxes.Add(TEXT("upperarm_l"), CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l")));
	_damageBoxes.Add(TEXT("upperarm_r"), CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r")));
	_damageBoxes.Add(TEXT("thigh_l"), CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l")));
	_damageBoxes.Add(TEXT("thigh_r"), CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r")));
}

void UDamageComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CacheBoxState();
}

void UDamageComponent::SetCharacter(class ACharacter* character)
{
	_character = character;

	for (auto& boxCom : _damageBoxes)
	{
		boxCom.Value->SetupAttachment(_character->GetMesh(), boxCom.Key);
		boxCom.Value->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Overlap);
		boxCom.Value->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void UDamageComponent::DebugDrawDamageBoxState(const TMap<FName, TSharedPtr<DamageBoxState>>& damageBoxStates, const FColor& color)
{
// 	for (auto& damageBoxState : damageBoxStates)
// 	{
// 		DrawDebugBox(GetWorld(), damageBoxState.Value->location, damageBoxState.Value->boxExtent, FQuat(damageBoxState.Value->rotation), color, true);
// 	}
	for (auto& boxCom : _damageBoxes)
	{
		DrawDebugBox(GetWorld(), boxCom.Value->GetComponentLocation(), boxCom.Value->GetScaledBoxExtent(), FQuat(boxCom.Value->GetComponentRotation()), color, true);
	}
}

bool UDamageComponent::RewindHitConfirm(double timeSeconds)
{
	if (0 == _damageBoxCaches.Num())
	{
		return false;
	}

	double headTimeSeconds = _damageBoxCaches.GetHead()->GetValue()->timeSeconds;
	double tailTimeSeconds = _damageBoxCaches.GetTail()->GetValue()->timeSeconds;
	Log(FString::Printf(TEXT("timeSeconds:%f, headTimeSeconds:%f, tailTimeSeconds:%f"), timeSeconds, headTimeSeconds, tailTimeSeconds));

	if (timeSeconds < _damageBoxCaches.GetHead()->GetValue()->timeSeconds)
	{
		// 比最早的数据还要早
		return false;
	}

	// 缓存当前状态
	CacheBoxState();
	//DebugDrawDamageBoxState(_damageBoxCaches.GetTail()->GetValue()->damageBoxStates, FColor::Blue);

	// 计算插值
	TSharedPtr<DamageBoxHistory> interpDamageBox;
	TSharedPtr<DamageBoxHistory> earlyDamageBox = _damageBoxCaches.GetHead()->GetValue();
	for (auto& damageBoxCache : _damageBoxCaches)
	{
		if (damageBoxCache->timeSeconds < timeSeconds)
		{
			earlyDamageBox = damageBoxCache;
		}
		else
		{
			// 计算插值
			interpDamageBox = MakeShared<DamageBoxHistory>();
			double ratio = (timeSeconds - earlyDamageBox->timeSeconds) / (damageBoxCache->timeSeconds - earlyDamageBox->timeSeconds);
			for (auto& boxCom : damageBoxCache->damageBoxStates)
			{
				TSharedPtr<DamageBoxState> state = MakeShared<DamageBoxState>();				
				state->location = FMath::VInterpTo(earlyDamageBox->damageBoxStates[boxCom.Key]->location, boxCom.Value->location, 1, ratio);
				state->rotation = FMath::RInterpTo(earlyDamageBox->damageBoxStates[boxCom.Key]->rotation, boxCom.Value->rotation, 1, ratio);
				state->boxExtent = FMath::VInterpTo(earlyDamageBox->damageBoxStates[boxCom.Key]->boxExtent, boxCom.Value->boxExtent, 1, ratio);
				
				interpDamageBox->damageBoxStates.Add(boxCom.Key, state);
			}

			break;
		}
	}

	// 验证碰撞
	TSet<AActor*> overlapingActors;
	for (auto& damageBoxState : _damageBoxes)
	{
		_damageBoxes[damageBoxState.Key]->SetWorldLocation(interpDamageBox->damageBoxStates[damageBoxState.Key]->location);
		_damageBoxes[damageBoxState.Key]->SetWorldRotation(interpDamageBox->damageBoxStates[damageBoxState.Key]->rotation);
		_damageBoxes[damageBoxState.Key]->SetBoxExtent(interpDamageBox->damageBoxStates[damageBoxState.Key]->boxExtent);
		_damageBoxes[damageBoxState.Key]->GetOverlappingActors(overlapingActors, ABullet::StaticClass());
// 		
// 		Log(FString::Printf(TEXT("boxExtent:%s realBoxExtent:%s"),
// 			*interpDamageBox->damageBoxStates[damageBoxState.Key]->boxExtent.ToString(),
// 			*_damageBoxes[damageBoxState.Key]->GetScaledBoxExtent().ToString()));
		if (overlapingActors.Num() > 0)
		{
			Log(TEXT("回溯击中！"));
			return true;
		}
	}

	return false;
}

void UDamageComponent::CacheBoxState()
{
	// 生成当前状态
	TSharedPtr<DamageBoxHistory> history = MakeShared<DamageBoxHistory>();
	history->timeSeconds = GetWorld()->GetTimeSeconds();
	for (auto& boxCom : _damageBoxes)
	{
		TSharedPtr<DamageBoxState> state = MakeShared<DamageBoxState>();
		state->location = boxCom.Value->GetComponentLocation();
		state->rotation = boxCom.Value->GetComponentRotation();
		state->boxExtent = boxCom.Value->GetUnscaledBoxExtent();
		history->damageBoxStates.Add(boxCom.Key, std::move(state));
	}
	//DebugDrawDamageBoxState(history->damageBoxStates, FColor::Orange);

	// 缓存当前状态
	if (_damageBoxCaches.Num() < 2)
	{
		_damageBoxCaches.AddTail(history);
	}
	else
	{
		while (_damageBoxCaches.GetTail()->GetValue()->timeSeconds - _damageBoxCaches.GetHead()->GetValue()->timeSeconds >= _cacheTime)
		{
			_damageBoxCaches.RemoveNode(_damageBoxCaches.GetHead());
		}
		_damageBoxCaches.AddTail(std::move(history));
	}
}