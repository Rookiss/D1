#pragma once

#include "D1PocketWorldSubsystem.generated.h"

class AD1PocketStage;
class UPocketLevelInstance;

DECLARE_DELEGATE_OneParam(FGetPocketStageDelegate, AD1PocketStage*);

UCLASS()
class UD1PocketWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UPocketLevelInstance* GetOrCreatePocketLevelFor(ULocalPlayer* LocalPlayer);
	
public:
	void RegisterAndCallForGetPocketStage(ULocalPlayer* LocalPlayer, FGetPocketStageDelegate Delegate);
	void SetPocketStage(AD1PocketStage* InPocketStage);
	
private:
	UPROPERTY()
	TWeakObjectPtr<AD1PocketStage> CachedPocketStage;
};
