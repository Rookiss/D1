#include "D1PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "D1GameplayTags.h"
#include "D1PlayerState.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Camera/D1PlayerCameraManager.h"
#include "Data/D1InputData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/D1InputComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"
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

	if (const UD1InputData* InputData = UD1AssetManager::GetInputData())
	{
		UD1InputComponent* D1InputComponent = CastChecked<UD1InputComponent>(InputComponent);

		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, InputBindHandles);
		
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_ChangeEquip_Primary, ETriggerEvent::Triggered, this, &ThisClass::Input_ChangeEquip_Primary, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_ChangeEquip_Secondary, ETriggerEvent::Triggered, this, &ThisClass::Input_ChangeEquip_Secondary, InputBindHandles);
		
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Ability_Confirm, ETriggerEvent::Triggered, this, &ThisClass::Input_LocalInputConfirm, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Ability_Cancel, ETriggerEvent::Triggered, this, &ThisClass::Input_LocalInputCancel, InputBindHandles);
		
		D1InputComponent->BindAbilityActions(InputData, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, InputBindHandles);
	}
}

void AD1PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

UAbilitySystemComponent* AD1PlayerController::GetAbilitySystemComponent() const
{
	if (AD1PlayerState* PS = GetPlayerState<AD1PlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AD1PlayerController::SetInputModeGameOnly()
{
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(true);
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
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			SetInputMode(InputMode);

			FlushPressedKeys();
			SetShowMouseCursor(bShowCursor);

			int32 X, Y;
			GetViewportSize(X, Y);
			SetMouseLocation(X / 2, Y / 2);
		}
	}
}

void AD1PlayerController::SetInputModeGameAndUI(bool bShowCursor)
{
	if (AD1HUD* D1HUD = Cast<AD1HUD>(GetHUD()))
	{
		if (UD1SceneWidget* SceneWidget = D1HUD->GetSceneWidget())
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(SceneWidget->TakeWidget());
			SetInputMode(InputMode);

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

void AD1PlayerController::Input_ChangeEquip_Primary()
{
	FGameplayEventData Payload;
	Payload.EventMagnitude = (int32)EWeaponEquipState::Primary;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, D1GameplayTags::Event_ChangeEquip, Payload);
}

void AD1PlayerController::Input_ChangeEquip_Secondary()
{
	FGameplayEventData Payload;
	Payload.EventMagnitude = (int32)EWeaponEquipState::Secondary;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, D1GameplayTags::Event_ChangeEquip, Payload);
}

void AD1PlayerController::Input_LocalInputConfirm()
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->LocalInputConfirm();
	}
}

void AD1PlayerController::Input_LocalInputCancel()
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->LocalInputCancel();
	}
}

void AD1PlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AD1PlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(GetAbilitySystemComponent()))
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
		InputBindHandles.Reset();
	}
}
