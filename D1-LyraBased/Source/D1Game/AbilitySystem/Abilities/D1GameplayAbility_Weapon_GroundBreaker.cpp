#include "D1GameplayAbility_Weapon_GroundBreaker.h"

#include "D1LogChannels.h"
#include "Character/LyraCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_GroundBreaker)

UD1GameplayAbility_Weapon_GroundBreaker::UD1GameplayAbility_Weapon_GroundBreaker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_GroundBreaker::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_GroundBreaker::Execute()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;
	
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return;

	float ScaledCapsuleRadius = LyraCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float ScaledCapsuleHalfHeight = LyraCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector Start = LyraCharacter->GetActorLocation() + (LyraCharacter->GetActorForwardVector() * DistanceOffset);
	FVector End = Start;
	FVector HalfSize = FVector(ScaledCapsuleRadius * 3.f, ScaledCapsuleRadius * 3.f, ScaledCapsuleHalfHeight);;
	FRotator Orientation = UKismetMathLibrary::MakeRotFromX(LyraCharacter->GetActorForwardVector());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) };
	TArray<AActor*> ActorsToIgnore = { LyraCharacter };
	
	TArray<FHitResult> OutHitResults;
	TMap<ALyraCharacter*, TPair<int32/*HitResult Index*/, float/*DistanceSq*/>> HitInfos;
	
	if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), Start, End, HalfSize, Orientation, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, OutHitResults, true))
	{
		for (int32 i = 0; i < OutHitResults.Num(); i++)
		{
			const FHitResult& HitResult = OutHitResults[i];
			
			ALyraCharacter* HitCharacter = Cast<ALyraCharacter>(HitResult.GetActor());
			if (HitCharacter == nullptr)
				continue;

			float DistanceSq = FVector::DistSquared(LyraCharacter->GetActorLocation(), HitResult.ImpactPoint);
			
			if (TPair<int32, float>* HitInfo = HitInfos.Find(HitCharacter))
			{
				float PrevDistanceSq = (*HitInfo).Value;
				if (DistanceSq < PrevDistanceSq)
				{
					(*HitInfo).Key = i;
					(*HitInfo).Value = DistanceSq;
				}
			}
			else
			{
				HitInfos.Add(HitCharacter, {i, DistanceSq});
			}
		}

		for (const auto& HitInfo : HitInfos)
		{
			const int32 HitResultIndex = HitInfo.Value.Key;
			const FHitResult& HitResult = OutHitResults[HitResultIndex];

			ALyraCharacter* TargetChacter = Cast<ALyraCharacter>(HitResult.GetActor());
			ProcessHitResult(HitResult, Damage, IsBlockingHit(TargetChacter), nullptr);
		}
	}
}
