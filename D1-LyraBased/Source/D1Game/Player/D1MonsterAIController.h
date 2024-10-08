#pragma once

#include "ModularAIController.h"
#include "Teams/D1TeamAgentInterface.h"
#include "D1MonsterAIController.generated.h"

UCLASS(Blueprintable)
class AD1MonsterAIController : public AModularAIController, public ID1TeamAgentInterface
{
	GENERATED_BODY()
	
public:
	AD1MonsterAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	
};
