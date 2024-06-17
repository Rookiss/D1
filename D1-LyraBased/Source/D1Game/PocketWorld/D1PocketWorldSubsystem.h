#pragma once

#include "D1PocketWorldSubsystem.generated.h"

class UPocketLevelInstance;
class AD1PocketStage;

DECLARE_DYNAMIC_DELEGATE_OneParam(FGetPocketStageDelegate, AD1PocketStage*, PocketStage);

UCLASS()
class UD1PocketWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UPocketLevelInstance* GetOrCreatePocketLevelFor(ULocalPlayer* LocalPlayer);
	
public:
	UFUNCTION(BlueprintCallable)
	void GetPocketStage(ULocalPlayer* LocalPlayer, FGetPocketStageDelegate Delegate);

	UFUNCTION(BlueprintCallable)
	void SetPocketStage(AD1PocketStage* InPocketStage);
	
private:
	UPROPERTY()
	TWeakObjectPtr<AD1PocketStage> CachedPocketStage;
};
