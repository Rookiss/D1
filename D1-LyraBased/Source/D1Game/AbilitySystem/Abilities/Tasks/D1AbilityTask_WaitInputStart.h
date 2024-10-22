#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_WaitInputStart.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputStartDelegate);

UCLASS()
class UD1AbilityTask_WaitInputStart : public UAbilityTask
{
	GENERATED_BODY()

public:
	UD1AbilityTask_WaitInputStart(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_WaitInputStart* WaitInputStart(UGameplayAbility* OwningAbility);
	
public:
	virtual void Activate() override;
	
public:
	UFUNCTION()
	void OnStartCallback();

public:
	UPROPERTY(BlueprintAssignable)
	FInputStartDelegate OnStart;
	
protected:
	FDelegateHandle DelegateHandle;
};
