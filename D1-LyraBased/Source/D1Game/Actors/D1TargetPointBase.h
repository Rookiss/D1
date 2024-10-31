#pragma once

#include "Engine/TargetPoint.h"
#include "D1TargetPointBase.generated.h"

class AAIController;

UENUM(BlueprintType)
enum class ED1TargetPointType : uint8
{
	None,
	Statue,
	Chest,
	Monster,
};

UCLASS()
class AD1TargetPointBase : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	AD1TargetPointBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void InitializeSpawningActor(AActor* InSpawningActor) { }
	
public:
	AActor* SpawnActor();
	void DestroyActor();

	AActor* GetSpawnedActor() const { return SpawnedActor.Get(); };
	
private:
	UFUNCTION()
	void OnExperienceLoaded(const ULyraExperienceDefinition* ExperienceDefinition);

public:
	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
	ED1TargetPointType TargetPointType = ED1TargetPointType::None;

	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
	bool bSpawnWhenDestroyed = false;
	
	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
	FVector SpawnLocationOffset = FVector::ZeroVector;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PreviewMeshComponent;
	
protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnedActor;
};
