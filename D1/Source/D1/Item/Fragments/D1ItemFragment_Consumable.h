#pragma once

#include "D1ItemFragment.h"
#include "D1ItemFragment_Consumable.generated.h"

class UGameplayEffect;

UCLASS(Const)
class UD1ItemFragment_Consumable : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Consumable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectToApply;
};
