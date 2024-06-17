#pragma once

#include "D1PocketWorldSubsystem.generated.h"

class UPocketLevelInstance;
class AD1PocketStage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPocketStageDelegate, AD1PocketStage*, PocketStage);

UCLASS()
class UD1PocketWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UPocketLevelInstance* GetOrCreatePocketLevelFor();
	
public:
	UFUNCTION(BlueprintCallable)
	void GetPocketStage(FPocketStageDelegate Delegate);

	UFUNCTION(BlueprintCallable)
	void SetPocketStage(AD1PocketStage* InPocketStage);
	
private:
	UPROPERTY()
	TWeakObjectPtr<AD1PocketStage> CachedPocketStage;
};
