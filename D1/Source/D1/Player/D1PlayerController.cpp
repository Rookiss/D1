#include "D1PlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "D1GameplayTags.h"
#include "D1PlayerState.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Data/D1InputData.h"
#include "Input/D1InputComponent.h"
#include "Inventory/D1InventoryManagerComponent.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerController)

AD1PlayerController::AD1PlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	InventoryManagerComponent = CreateDefaultSubobject<UD1InventoryManagerComponent>("InventoryManagerComponent");
}

void AD1PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UD1InputData* InputData = UD1AssetManager::GetAssetByName<UD1InputData>("InputData_Default"))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputData->MappingContext, 0);
		}
	}

	// @TODO: For Test
	if (HasAuthority())
	{
		InventoryManagerComponent->Server_Init();
		InventoryManagerComponent->TryAddItemByID(1003, 1);
		InventoryManagerComponent->TryAddItemByID(1002, 1);
		InventoryManagerComponent->TryAddItemByID(1001, 1);
		InventoryManagerComponent->TryAddItemByID(1003, 1);
		InventoryManagerComponent->TryAddItemByID(1001, 1);
		InventoryManagerComponent->TryAddItemByID(1002, 1);
	}
}

void AD1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const UD1InputData* InputData = UD1AssetManager::GetAssetByName<UD1InputData>("InputData_Default"))
	{
		UD1InputComponent* D1InputComponent = CastChecked<UD1InputComponent>(InputComponent);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, InputBindHandles);
		D1InputComponent->BindNativeAction(InputData, D1GameplayTags::Input_Action_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, InputBindHandles);
		D1InputComponent->BindAbilityActions(InputData, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, InputBindHandles);
	}
}

void AD1PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UD1AbilitySystemComponent* D1ASC = Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState)))
	{
		D1ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
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

void AD1PlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* D1ASC = Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState)))
	{
		D1ASC->AbilityInputTagPressed(InputTag);
	}
}

void AD1PlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetPawn() == nullptr)
		return;
	
	if (UD1AbilitySystemComponent* D1ASC = Cast<UD1AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState)))
	{
   		D1ASC->AbilityInputTagReleased(InputTag);
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
