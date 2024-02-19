#include "D1PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "D1GameplayTags.h"
#include "D1PlayerState.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Camera/D1PlayerCameraManager.h"
#include "Data/D1InputData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/D1InputComponent.h"
#include "System/D1AssetManager.h"
#include "UI/D1HUD.h"
#include "UI/D1SceneWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerController)

AD1PlayerController::AD1PlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	PlayerCameraManagerClass = AD1PlayerCameraManager::StaticClass();
}

void AD1PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UD1InputData* InputData = UD1AssetManager::GetAssetByName<UD1InputData>("InputData"))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputData->InputMappingContext, 0);
		}
	}
}

void AD1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const UD1InputData* InputData = UD1AssetManager::GetAssetByName<UD1InputData>("InputData"))
	{
		UD1InputComponent* D1InputComponent = CastChecked<UD1InputComponent>(InputComponent);

		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Inventory, ETriggerEvent::Triggered, this, &ThisClass::Input_Inventory, InputBindHandles);
		
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_EquipWeapon_Primary, ETriggerEvent::Triggered, this, &ThisClass::Input_EquipWeapon_Primary, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_EquipWeapon_Secondary, ETriggerEvent::Triggered, this, &ThisClass::Input_EquipWeapon_Secondary, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_EquipWeapon_CycleBackward, ETriggerEvent::Triggered, this, &ThisClass::Input_EquipWeapon_CycleBackward, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_EquipWeapon_CycleForward, ETriggerEvent::Triggered, this, &ThisClass::Input_EquipWeapon_CycleForward, InputBindHandles);

		D1InputComponent->BindAbilityActions(InputData, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, InputBindHandles);
	}
}

void AD1PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState)))
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AD1PlayerController::SetInputModeGameOnly()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	FlushPressedKeys();
	SetShowMouseCursor(false);
}

void AD1PlayerController::SetInputModeUIOnly(bool bShowCursor)
{
	if (AD1HUD* D1HUD = Cast<AD1HUD>(GetHUD()))
	{
		if (UD1SceneWidget* SceneWidget = D1HUD->GetSceneWidget())
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(SceneWidget->TakeWidget());
			SetInputMode(InputMode);
	
			FlushPressedKeys();
			SetShowMouseCursor(bShowCursor);
		}
	}
}

void AD1PlayerController::Input_Move(const FInputActionValue& InputValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D Value = InputValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		
		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			ControlledPawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			ControlledPawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void AD1PlayerController::Input_Look(const FInputActionValue& InputValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D Value = InputValue.Get<FVector2D>();
		
		if (Value.X != 0.0f)
		{
			ControlledPawn->AddControllerYawInput(Value.X);
		}

		if (Value.Y != 0.0f)
		{
			ControlledPawn->AddControllerPitchInput(Value.Y);
		}
	}
}

void AD1PlayerController::Input_Crouch()
{
	if (ACharacter* ControlledCharacter = GetCharacter())
	{
		if (ControlledCharacter->GetCharacterMovement()->IsFalling() == false)
		{
			(ControlledCharacter->bIsCrouched) ? ControlledCharacter->UnCrouch() : ControlledCharacter->Crouch();
		}
	}
}

void AD1PlayerController::Input_Inventory()
{
	if (AD1HUD* HUD = Cast<AD1HUD>(GetHUD()))
	{
		HUD->ShowControlledInventoryWidget();
	}
}

void AD1PlayerController::Input_EquipWeapon_Primary()
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, D1GameplayTags::Event_EquipWeapon_Primary, FGameplayEventData());
}

void AD1PlayerController::Input_EquipWeapon_Secondary()
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, D1GameplayTags::Event_EquipWeapon_Secondary, FGameplayEventData());
}

void AD1PlayerController::Input_EquipWeapon_CycleBackward()
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, D1GameplayTags::Event_EquipWeapon_CycleBackward, FGameplayEventData());
}

void AD1PlayerController::Input_EquipWeapon_CycleForward()
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, D1GameplayTags::Event_EquipWeapon_CycleForward, FGameplayEventData());
}

void AD1PlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState)))
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AD1PlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState)))
	{
   		ASC->AbilityInputTagReleased(InputTag);
	}
}

void AD1PlayerController::ResetInput()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
	}

	if (UD1InputComponent* D1InputComponent = Cast<UD1InputComponent>(InputComponent))
	{
		D1InputComponent->RemoveBinds(InputBindHandles);
	}
}
