#include "D1ObjectManagerSubsystem.h"

#include "Actors/D1TargetPointBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ObjectManagerSubsystem)

UD1ObjectManagerSubsystem::UD1ObjectManagerSubsystem()
{

}

void UD1ObjectManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(&InWorld, AD1TargetPointBase::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		AD1TargetPointBase* TargetPointBase = Cast<AD1TargetPointBase>(Actor);
		switch (TargetPointBase->TargetPointType)
		{
		case ED1TargetPointType::Statue:	PoolStatueTargetPoints.Add(TargetPointBase);	break;
		case ED1TargetPointType::Chest:		PoolChestTargetPoints.Add(TargetPointBase);		break;
		}
	}
	
	int32 StatueSpawnCount = PoolStatueTargetPoints.Num() < 4 ? PoolStatueTargetPoints.Num() : PoolStatueTargetPoints.Num() / 4;
	SpawnToRandomTargetPoint(PoolStatueTargetPoints, UsingStatueTargetPoints, StatueSpawnCount);

	int32 ChestSpawnCount = PoolChestTargetPoints.Num();
	SpawnToRandomTargetPoint(PoolChestTargetPoints, UsingChestTargetPoints, ChestSpawnCount);
}

bool UD1ObjectManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (Super::ShouldCreateSubsystem(Outer) == false)
		return false;

	UWorld* World = Cast<UWorld>(Outer);
	return World->GetNetMode() < NM_Client;
}

void UD1ObjectManagerSubsystem::OnActorDestroyed(AActor* DestroyedActor)
{
	TObjectPtr<AD1TargetPointBase>* TargetPointBasePtr = SpawnedActorToTargetPoint.Find(DestroyedActor);
	if (TargetPointBasePtr == nullptr)
		return;

	AD1TargetPointBase* TargetPointBase = TargetPointBasePtr->Get();
	if (TargetPointBase->TargetPointType == ED1TargetPointType::Statue)
	{
		SpawnToRandomTargetPoint(PoolStatueTargetPoints, UsingStatueTargetPoints);
		UsingStatueTargetPoints.Remove(TargetPointBase);
		PoolStatueTargetPoints.Add(TargetPointBase);
	}
}

void UD1ObjectManagerSubsystem::SpawnToRandomTargetPoint(TArray<TObjectPtr<AD1TargetPointBase>>& PoolTargetPoints, TSet<TObjectPtr<AD1TargetPointBase>>& UsingTargetPoints, int32 SpawnCount)
{
	for (int32 i = 0; i < SpawnCount; i++)
	{
		int32 SelectedIndex = UKismetMathLibrary::RandomInteger(PoolTargetPoints.Num());

		AActor* SpawnedActor = PoolTargetPoints[SelectedIndex]->SpawnActor();
		SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnActorDestroyed);
		SpawnedActorToTargetPoint.Add(SpawnedActor, PoolTargetPoints[SelectedIndex]);
		
		UsingTargetPoints.Add(PoolTargetPoints[SelectedIndex]);
		PoolTargetPoints.RemoveAtSwap(SelectedIndex);
	}
}
