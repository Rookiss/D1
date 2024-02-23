#pragma once

#include "AbilitySystemInterface.h"
#include "D1PlayerController.generated.h"

struct FGameplayTag;
struct FInputActionValue;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

UCLASS()
class AD1PlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PostInitializeComponents() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

protected:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
public:
	void SetInputModeGameOnly();
	void SetInputModeUIOnly(bool bShowCursor = true);
	
private:
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);
	void Input_Crouch();
	void Input_Inventory();

	void Input_EquipWeapon_ToggleArming();
	void Input_EquipWeapon_Primary();
	void Input_EquipWeapon_Secondary();
	void Input_EquipWeapon_CycleBackward();
	void Input_EquipWeapon_CycleForward();
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	void ResetInput();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;
	
private:
	TArray<uint32> InputBindHandles;
};
