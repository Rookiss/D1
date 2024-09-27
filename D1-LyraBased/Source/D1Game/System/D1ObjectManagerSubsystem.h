#pragma once

#include "D1ObjectManagerSubsystem.generated.h"

class AD1TargetPointBase;

UCLASS()
class UD1ObjectManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UD1ObjectManagerSubsystem();

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

private:
	UFUNCTION()
	void OnActorDestroyed(AActor* DestroyedActor);

	void SpawnToRandomTargetPoint(TArray<TObjectPtr<AD1TargetPointBase>>& PoolTargetPoints, TSet<TObjectPtr<AD1TargetPointBase>>& UsingTargetPoints, int32 SpawnCount = 1);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<AD1TargetPointBase>> PoolStatueTargetPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TSet<TObjectPtr<AD1TargetPointBase>> UsingStatueTargetPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<AD1TargetPointBase>> PoolChestTargetPoints;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TSet<TObjectPtr<AD1TargetPointBase>> UsingChestTargetPoints;

private:
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<AActor>, TObjectPtr<AD1TargetPointBase>> SpawnedActorToTargetPoint;
};
