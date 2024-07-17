#pragma once

#include "D1Interactable.h"
#include "D1WorldInteractable.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class AD1WorldInteractable : public AActor, public ID1Interactable
{
	GENERATED_BODY()
	
public:
	AD1WorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionStarted();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Interaction Started"))
	void K2_OnInteractionStarted();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionFailed();
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Interaction Failed"))
	void K2_OnInteractionFailed();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionSucceeded();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Interaction Succeeded"))
	void K2_OnInteractionSucceeded();
};
