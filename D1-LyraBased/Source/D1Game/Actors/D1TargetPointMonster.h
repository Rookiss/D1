#pragma once

#include "D1TargetPointBase.h"
#include "D1TargetPointMonster.generated.h"

class AAIController;

UCLASS()
class AD1TargetPointMonster : public AD1TargetPointBase
{
	GENERATED_BODY()
	
public:
	AD1TargetPointMonster(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeSpawningActor(AActor* InSpawningActor) override;
};
