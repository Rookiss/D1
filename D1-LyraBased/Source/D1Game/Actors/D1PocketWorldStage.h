#pragma once

#include "D1PocketWorldStage.generated.h"

UCLASS()
class AD1PocketWorldStage : public AActor
{
	GENERATED_BODY()
	
public:
	AD1PocketWorldStage(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PreCaptureDiffuse();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PostCaptureDiffuse();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PreCaptureAlphaMask();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PostCaptureAlphaMask();
};
