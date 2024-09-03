#include "D1GameplayAbility_Weapon_WhirlwindSlash.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_WhirlwindSlash)

UD1GameplayAbility_Weapon_WhirlwindSlash::UD1GameplayAbility_Weapon_WhirlwindSlash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_WhirlwindSlash::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	ULyraAbilitySystemComponent* SourceASC = GetLyraAbilitySystemComponentFromActorInfo();
	check(SourceASC);

	if (SourceASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow	ScopedPrediction(SourceASC, GetCurrentActivationInfo().GetActivationPredictionKey());
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InTargetDataHandle)));

		TArray<int32> AttackHitIndexes;
		TArray<int32> BlockHitIndexes;
		ParseTargetData(LocalTargetDataHandle, AttackHitIndexes, BlockHitIndexes);

		for (int32 BlockHitIndex : BlockHitIndexes)
		{
			FHitResult HitResult = *(LocalTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
			ProcessHitResult(HitResult, Damage, true, nullptr);
		}

		for (int32 AttackHitIndex : AttackHitIndexes)
		{
			FHitResult HitResult = *LocalTargetDataHandle.Data[AttackHitIndex]->GetHitResult();
			ProcessHitResult(HitResult, Damage, false, nullptr);
		}
	}
}
