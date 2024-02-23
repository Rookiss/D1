#pragma once

#include "D1Define.h"
#include "D1GameplayAbility.h"
#include "D1GameplayAbility_ChangeWeaponEquipState.generated.h"

class UD1EquipmentManagerComponent;

UCLASS()
class UD1GameplayAbility_ChangeWeaponEquipState : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_ChangeWeaponEquipState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
};
