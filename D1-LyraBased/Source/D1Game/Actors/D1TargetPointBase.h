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
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	AActor* SpawnActor();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void DestroyActor();
	
public:
	UPROPERTY(EditDefaultsOnly)
	ED1TargetPointType TargetPointType = ED1TargetPointType::None;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SpawnActorClass;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> SpawnedActor;	
};
