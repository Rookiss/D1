#pragma once

#include "GameplayEffectTypes.h"
#include "D1AOEBase.generated.h"

UCLASS(BlueprintType, Abstract)
class AD1AOEBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1AOEBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
};
