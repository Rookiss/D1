#pragma once

#include "D1PlayerController.generated.h"

class UD1UserWidget;
struct FInputActionValue;
struct FGameplayTag;

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

public:
	void SetInputModeGameOnly();
	void SetInputModeUIOnly(bool bShowCursor = true);
	
private:
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);
	void Input_Crouch();
	void Input_Inventory();
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	void ResetInput();

private:
	TArray<uint32> InputBindHandles;
};
