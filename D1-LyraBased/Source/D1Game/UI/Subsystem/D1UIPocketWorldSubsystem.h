#pragma once

#include "D1UIPocketWorldSubsystem.generated.h"

class UPocketLevelInstance;
class UPocketCapture;
class AD1PocketWorldStage;

UCLASS()
class UD1UIPocketWorldSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UPocketLevelInstance* GetOrCreatePocketLevelFor(ULocalPlayer* LocalPlayer);
	
public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UPocketCapture> PocketCapture;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AD1PocketWorldStage> PocketWorldStage;
};
