#pragma once

#include "Engine/TargetPoint.h"
#include "D1TargetPointBase.generated.h"

UENUM(BlueprintType)
enum class ED1TargetPointType : uint8
{
	None,
	Statue,
	Chest,
};

UCLASS()
class AD1TargetPointBase : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	AD1TargetPointBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	AActor* SpawnActor();
	void DestroyActor();

protected:
	virtual void InitializeSpawningActor(AActor* InSpawningActor);
	
public:
	UPROPERTY(EditDefaultsOnly)
	ED1TargetPointType TargetPointType = ED1TargetPointType::None;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SpawnActorClass;
	
protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnedActor;
};
