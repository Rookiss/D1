#pragma once

#include "D1TargetPointBase.h"
#include "D1ChestTargetPoint.generated.h"

class UD1ItemTemplate;

UCLASS()
class AD1ChestTargetPoint : public AD1TargetPointBase
{
	GENERATED_BODY()
	
public:
	AD1ChestTargetPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeSpawningActor(AActor* InSpawningActor) override;
};
