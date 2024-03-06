#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "D1AbilityTask_SendAttackTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDataSet, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

UCLASS()
class UD1AbilityTask_SendAttackTargetData : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Send Attack Target Data", HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UD1AbilityTask_SendAttackTargetData* SendAttackTargetData(UGameplayAbility* OwningAbility);

protected:
	virtual void Activate() override;

private:
	void SendTargetDataToServer();
	void OnTargetDataSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag);
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnTargetDataSet TargetDataSet;
};
