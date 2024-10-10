#include "D1ActorSpawningManagerComponent.h"

#include "Actors/D1TargetPointBase.h"
#include "GameModes/LyraExperienceManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ActorSpawningManagerComponent)

UD1ActorSpawningManagerComponent::UD1ActorSpawningManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1ActorSpawningManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UD1ActorSpawningManagerComponent::OnExperienceLoaded(const ULyraExperienceDefinition* ExperienceDefinition)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		ServerSpawnActors();
	}
#endif
}

void UD1ActorSpawningManagerComponent::ServerSpawnActors()
{
#if WITH_SERVER_CODE
	TArray<AActor*> TargetPointActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AD1TargetPointBase::StaticClass(), TargetPointActors);

	for (AActor* TargetPointActor : TargetPointActors)
	{
		AD1TargetPointBase* TargetPointBase = Cast<AD1TargetPointBase>(TargetPointActor);
		switch (TargetPointBase->TargetPointType)
		{
		case ED1TargetPointType::Statue:	StatueTargetPointList.PendingTargetPoints.Add(TargetPointBase);		break;
		case ED1TargetPointType::Chest:		ChestTargetPointList.PendingTargetPoints.Add(TargetPointBase);		break;
		case ED1TargetPointType::Monster:	MonsterTargetPointList.PendingTargetPoints.Add(TargetPointBase);	break;
		}
	}

	// Spawn Statues
	const int32 PendingStatueTargetPointCount = StatueTargetPointList.PendingTargetPoints.Num();
	const int32 StatueSpawnCount = PendingStatueTargetPointCount < 4 ? PendingStatueTargetPointCount : PendingStatueTargetPointCount / 4;
	SpawnToRandomTargetPoints(StatueTargetPointList, StatueSpawnCount);

	// Spawn Chests
	SpawnToAllTargetPoints(ChestTargetPointList);

	// Spawn Monsters
	SpawnToAllTargetPoints(MonsterTargetPointList);
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
		SpawnToRandomTargetPoints(*PendingList, 1);
	}
	
	PendingList->PendingTargetPoints.Add(TargetPoint);
	SpawnedActorToTargetPoint.Remove(DestroyedActor);
#endif
}

void UD1ActorSpawningManagerComponent::SpawnToRandomTargetPoints(FPendingTargetPointList& InTargetPointList, int32 InSpawnCount)
{
#if WITH_SERVER_CODE
	TArray<TObjectPtr<AD1TargetPointBase>>& PendingTargetPoints = InTargetPointList.PendingTargetPoints;
	
	for (int32 i = 0; i < InSpawnCount; i++)
	{
		int32 SelectedIndex = UKismetMathLibrary::RandomInteger(PendingTargetPoints.Num());
		SpawnToTargetPoint(InTargetPointList, SelectedIndex);
		PendingTargetPoints.RemoveAtSwap(SelectedIndex);
	}
#endif
}

void UD1ActorSpawningManagerComponent::SpawnToAllTargetPoints(FPendingTargetPointList& InTargetPointList)
{
#if WITH_SERVER_CODE
	TArray<TObjectPtr<AD1TargetPointBase>>& PendingTargetPoints = InTargetPointList.PendingTargetPoints;
	
	for (int i = 0; i < PendingTargetPoints.Num(); i++)
	{
		SpawnToTargetPoint(InTargetPointList, i);
	}
	
	PendingTargetPoints.Empty();
#endif
}

void UD1ActorSpawningManagerComponent::SpawnToTargetPoint(FPendingTargetPointList& InTargetPointList, int32 InTargetPointIndex)
{
#if WITH_SERVER_CODE
	TArray<TObjectPtr<AD1TargetPointBase>>& PendingTargetPoints = InTargetPointList.PendingTargetPoints;
	if (PendingTargetPoints.IsValidIndex(InTargetPointIndex) == false)
		return;

	AD1TargetPointBase* PendingTargetPoint = PendingTargetPoints[InTargetPointIndex];
	if (AActor* SpawnedActor = PendingTargetPoint->SpawnActor())
	{
		SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnActorDestroyed);
		SpawnedActorToTargetPoint.Emplace(SpawnedActor, FUsingTargetPointEntry(PendingTargetPoint, &InTargetPointList));
	}
#endif
}
