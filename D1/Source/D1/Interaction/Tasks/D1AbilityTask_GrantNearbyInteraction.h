#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_GrantNearbyInteraction.generated.h"

UCLASS()
class UD1AbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
public:
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwingAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_GrantNearbyInteraction* GrantNearbyInteraction(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate);

private:
	void QueryInteractables();

private:
	FTimerHandle QueryTimerHandle;
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> AppliedInteractionAbilities;
};
