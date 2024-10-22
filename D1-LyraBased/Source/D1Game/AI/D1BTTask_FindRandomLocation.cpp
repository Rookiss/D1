#include "D1BTTask_FindRandomLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "GameFramework/PawnMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1BTTask_FindRandomLocation)

UD1BTTask_FindRandomLocation::UD1BTTask_FindRandomLocation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("FInd Random Location");
}

EBTNodeResult::Type UD1BTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	if (AIController == nullptr || AIController->GetPawn() == nullptr)
		return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
		return EBTNodeResult::Failed;
	
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavigationSystem == nullptr)
		return EBTNodeResult::Failed;
	
	FNavLocation NavLocation;
	FVector Origin = ControlledPawn->GetActorLocation();
	FVector Destination = Origin;
	
	if (NavigationSystem->GetRandomReachablePointInRadius(Origin, Radius, NavLocation, nullptr))
	{
		Destination = NavLocation.Location;
	}

	Blackboard->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Destination);
	DrawDebugSphere(GetWorld(), Destination, 25.f, 12, FColor::Red, false, 10.f);
	
	return EBTNodeResult::Succeeded;
}
