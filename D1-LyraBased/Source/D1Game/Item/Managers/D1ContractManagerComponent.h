#pragma once

#include "Components/PlayerStateComponent.h"
#include "D1ContractManagerComponent.generated.h"

UCLASS()
class UD1ContractManagerComponent : public UPlayerStateComponent
{
	GENERATED_BODY()
	
public:
	UD1ContractManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
