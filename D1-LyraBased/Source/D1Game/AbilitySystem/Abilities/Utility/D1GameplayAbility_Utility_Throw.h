#pragma once

#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Utility_Throw.generated.h"

class AD1PickupableItemBase;

UCLASS()
class UD1GameplayAbility_Utility_Throw : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Utility_Throw(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTransform GetSpawnTransform();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SpawnThrowableItem();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AD1PickupableItemBase> ThrowableItemClass;
};
