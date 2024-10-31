#pragma once

#include "Components/GameStateComponent.h"
#include "D1ActorSpawningManagerComponent.generated.h"

class ULyraExperienceDefinition;
class AD1TargetPointBase;

USTRUCT()
struct FPendingTargetPointList
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AD1TargetPointBase>> PendingTargetPoints;
};

USTRUCT()
struct FUsingTargetPointEntry
{
	GENERATED_BODY()

public:
	FUsingTargetPointEntry() { }
	
	FUsingTargetPointEntry(AD1TargetPointBase* InTargetPoint, FPendingTargetPointList* InPendingList)
		: TargetPoint(InTargetPoint), PendingList(InPendingList) { }

public:
	UPROPERTY(Transient)
	TObjectPtr<AD1TargetPointBase> TargetPoint = nullptr;

	FPendingTargetPointList* PendingList = nullptr;
};

UCLASS()
class UD1ActorSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:
	UD1ActorSpawningManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RegisterTargetPoint(AD1TargetPointBase* InTargetPoint);
	
private:
	void SpawnToTargetPoint(FPendingTargetPointList& InTargetPointList, AD1TargetPointBase* InTargetPoint);
	
	UFUNCTION()
	void OnActorDestroyed(AActor* DestroyedActor);
	
private:
	UPROPERTY(Transient)
	FPendingTargetPointList StatueTargetPointList;

	UPROPERTY(Transient)
	FPendingTargetPointList ChestTargetPointList;

	UPROPERTY(Transient)
	FPendingTargetPointList MonsterTargetPointList;
	
private:
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<AActor>, FUsingTargetPointEntry> SpawnedActorToTargetPoint;
};
