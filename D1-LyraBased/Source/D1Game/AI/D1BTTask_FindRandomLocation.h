#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "D1BTTask_FindRandomLocation.generated.h"

UCLASS()
class UD1BTTask_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UD1BTTask_FindRandomLocation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category="D1")
	float Radius = 1000.f;
};
