#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "D1PlayerController.generated.h"

struct FGameplayTag;
struct FInputActionValue;

UCLASS()
class AD1PlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
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
	
	void Input_EquipWeapon_Primary();
	void Input_EquipWeapon_Secondary();
	void Input_EquipWeapon_CycleBackward();
	void Input_EquipWeapon_CycleForward();
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	void ResetInput();
	
private:
	TArray<uint32> InputBindHandles;
};
