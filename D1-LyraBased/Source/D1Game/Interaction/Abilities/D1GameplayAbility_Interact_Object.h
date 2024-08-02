#pragma once

#include "D1GameplayAbility_Interact_Info.h"
#include "D1GameplayAbility_Interact_Object.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Object : public UD1GameplayAbility_Interact_Info
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Object(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
