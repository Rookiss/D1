#include "D1ActorSpawningManagerComponent.h"

#include "Actors/D1TargetPointBase.h"
#include "GameModes/LyraExperienceManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ActorSpawningManagerComponent)

UD1ActorSpawningManagerComponent::UD1ActorSpawningManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1ActorSpawningManagerComponent::RegisterTargetPoint(AD1TargetPointBase* InTargetPoint)
{
#if WITH_SERVER_CODE
	if (InTargetPoint == nullptr)
		return;

	switch (InTargetPoint->TargetPointType)
	{
	case ED1TargetPointType::Statue:	SpawnToTargetPoint(StatueTargetPointList, InTargetPoint);		break;
	case ED1TargetPointType::Chest:		SpawnToTargetPoint(ChestTargetPointList, InTargetPoint);		break;
	case ED1TargetPointType::Monster:	SpawnToTargetPoint(MonsterTargetPointList, InTargetPoint);		break;
	}
#endif
}

void UD1ActorSpawningManagerComponent::SpawnToTargetPoint(FPendingTargetPointList& InTargetPointList, AD1TargetPointBase* InTargetPoint)
{
#if WITH_SERVER_CODE
	if (InTargetPoint == nullptr)
		return;

	if (AActor* SpawnedActor = InTargetPoint->SpawnActor())
	{
		SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnActorDestroyed);
		SpawnedActorToTargetPoint.Add(SpawnedActor, FUsingTargetPointEntry(InTargetPoint, &InTargetPointList));
	}
#endif
}

void UD1ActorSpawningManagerComponent::OnActorDestroyed(AActor* DestroyedActor)
{
#if WITH_SERVER_CODE
	FUsingTargetPointEntry* UsingTargetPointEntry = SpawnedActorToTargetPoint.Find(DestroyedActor);
	if (UsingTargetPointEntry == nullptr)
		return;

	AD1TargetPointBase* TargetPoint = UsingTargetPointEntry->TargetPoint;
	FPendingTargetPointList* PendingList = UsingTargetPointEntry->PendingList;
	if (TargetPoint == nullptr || PendingList == nullptr)
		return;
	
	if (TargetPoint->bSpawnWhenDestroyed)
	{
		SpawnToTargetPoint(*PendingList, TargetPoint);
	}
	
	PendingList->PendingTargetPoints.Add(TargetPoint);
	SpawnedActorToTargetPoint.Remove(DestroyedActor);
#endif
}
