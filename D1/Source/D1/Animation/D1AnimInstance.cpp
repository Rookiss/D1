#include "D1AnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "KismetAnimationLibrary.h"
#include "Character/D1Player.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Player/D1PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimInstance)

UD1AnimInstance::UD1AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializedWithAbilitySystem(ASC);
		}
	}
}

void UD1AnimInstance::InitializedWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	GameplayTagPropertyMap.Initialize(this, ASC);
}

void UD1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (AD1Player* Character = Cast<AD1Player>(TryGetPawnOwner()))
	{
		AimPitch = Character->CalculateAimPitch();
			
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			Velocity = MovementComponent->Velocity;
			GroundSpeed = Velocity.Size2D();
			Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
				
			bShouldMove = (GroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);
			bIsCrouching = MovementComponent->IsCrouching();
			bIsFalling = MovementComponent->IsFalling();
		}
		
		LeftHandWeight = 0.f;
		RightHandWeight = 0.f;
		
		UD1EquipManagerComponent* EquipManager = Character->EquipManagerComponent;
		EWeaponEquipState WeaponEquipState = EquipManager->GetCurrentWeaponEquipState();
		const TArray<EEquipmentSlotType>& SlotTypes = Item::EquipmentSlotsByWeaponState[(int32)WeaponEquipState];

		for (EEquipmentSlotType SlotType : SlotTypes)
		{
			const TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			const FD1EquipEntry& Entry = Entries[(int32)SlotType];

			if (Entry.GetItemInstance())
			{
				switch (SlotType)
				{
				case EEquipmentSlotType::Unarmed:
				case EEquipmentSlotType::Primary_TwoHand:
				case EEquipmentSlotType::Secondary_TwoHand:
					LeftHandWeight = 1.f;
					RightHandWeight = 1.f;
					break;
				case EEquipmentSlotType::Primary_LeftHand:
				case EEquipmentSlotType::Secondary_LeftHand:
					LeftHandWeight = 1.f;
					break;
				case EEquipmentSlotType::Primary_RightHand:
				case EEquipmentSlotType::Secondary_RightHand:
					RightHandWeight = 1.f;
					break;
				}
			}
		}
	}
}
