#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_GrantNearbyInteractionAbilities.generated.h"

UCLASS()
class UD1AbilityTask_GrantNearbyInteractionAbilities : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_GrantNearbyInteractionAbilities(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwingAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_GrantNearbyInteractionAbilities* GrantNearbyInteractionAbilities(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void QueryInteractables();

private:
	FTimerHandle QueryTimerHandle;
	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> GrantedInteractionAbilities;
};
