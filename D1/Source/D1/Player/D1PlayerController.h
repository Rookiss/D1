#pragma once

#include "AbilitySystemInterface.h"
#include "D1PlayerController.generated.h"

struct FInputActionValue;
struct FGameplayTag;
class UD1AbilitySystemComponent;
class UD1InventoryManagerComponent;

UCLASS()
class AD1PlayerController : public APlayerController, public IAbilitySystemInterface
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

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;

private:
	TArray<uint32> InputBindHandles;
};
