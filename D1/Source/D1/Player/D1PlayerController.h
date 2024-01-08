#pragma once

#include "D1PlayerController.generated.h"

struct FInputActionValue;
struct FGameplayTag;
class AD1PlayerState;
class UD1AbilitySystemComponent;

UCLASS()
class AD1PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AD1PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
private:
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	void ResetInput();
	
private:
	TArray<uint32> InputBindHandles;
};
