#pragma once

#include "D1Character.h"
#include "D1Monster.generated.h"

UCLASS()
class AD1Monster : public AD1Character
{
	GENERATED_BODY()
	
public:
	AD1Monster(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void InitAbilitySystem() override;
};
