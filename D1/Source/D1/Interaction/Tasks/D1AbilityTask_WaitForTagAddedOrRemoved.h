#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_WaitForTagAddedOrRemoved.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTagAddedOrRemoved, const FGameplayTag&, Tag);

UCLASS()
class UD1AbilityTask_WaitForTagAddedOrRemoved : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UD1AbilityTask_WaitForTagAddedOrRemoved(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_WaitForTagAddedOrRemoved* WaitForTagAddedOrRemoved(UGameplayAbility* OwningAbility, FGameplayTagContainer InListenTags);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void OnTagChanged(const FGameplayTag Tag, int32 NewCount);
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnTagAddedOrRemoved OnTagAdded;

	UPROPERTY(BlueprintAssignable)
	FOnTagAddedOrRemoved OnTagRemoved;

private:
	UPROPERTY()
	FGameplayTagContainer ListenTags;
};
