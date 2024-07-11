#pragma once

#include "LyraGameplayAbility.h"
#include "LyraGameplayAbility_Jump.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

UCLASS(Abstract)
class ULyraGameplayAbility_Jump : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	ULyraGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	void CharacterJumpStart();
};
