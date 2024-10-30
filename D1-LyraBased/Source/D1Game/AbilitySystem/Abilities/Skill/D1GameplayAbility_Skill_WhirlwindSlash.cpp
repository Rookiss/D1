#include "D1GameplayAbility_Skill_WhirlwindSlash.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_WhirlwindSlash)

UD1GameplayAbility_Skill_WhirlwindSlash::UD1GameplayAbility_Skill_WhirlwindSlash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Skill_WhirlwindSlash::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	ULyraAbilitySystemComponent* SourceASC = GetLyraAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;

	AD1WeaponBase* WeaponActor = GetFirstWeaponActor();
	if (WeaponActor == nullptr)
		return;

	if (SourceASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InTargetDataHandle)));

		TArray<int32> CharacterHitIndexes;
		TArray<int32> BlockHitIndexes;
		ParseTargetData(LocalTargetDataHandle, CharacterHitIndexes, BlockHitIndexes);

		for (int32 BlockHitIndex : BlockHitIndexes)
		{
			FHitResult HitResult = *(LocalTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
			ProcessHitResult(HitResult, Damage, true, nullptr, WeaponActor);
		}

		for (int32 CharacterHitIndex : CharacterHitIndexes)
		{
			FHitResult HitResult = *LocalTargetDataHandle.Data[CharacterHitIndex]->GetHitResult();
			ProcessHitResult(HitResult, Damage, false, nullptr, WeaponActor);
		}
	}
}
