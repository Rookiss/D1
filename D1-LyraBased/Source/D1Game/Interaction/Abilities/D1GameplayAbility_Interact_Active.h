#pragma once

#include "D1GameplayAbility_Interact_Info.h"
#include "D1GameplayAbility_Interact_Active.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Active : public UD1GameplayAbility_Interact_Info
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	bool TriggerInteraction();
};
