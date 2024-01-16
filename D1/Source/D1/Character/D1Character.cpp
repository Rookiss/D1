#include "D1Character.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/D1AbilitySystemData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Character)

AD1Character::AD1Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetCullDistanceSquared = 900000000.0f;
	
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);
	
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void AD1Character::InitAbilityActorInfo()
{
	
}

void AD1Character::ApplyAbilitySystemData(const FName& DataName)
{
	if (const UD1AbilitySystemData* Data = UD1AssetManager::GetAssetByName<UD1AbilitySystemData>(DataName))
	{
		Data->GiveToAbilitySystem(D1AbilitySystemComponent, &GrantedHandles);
	}
}

UAbilitySystemComponent* AD1Character::GetAbilitySystemComponent() const
{
	return D1AbilitySystemComponent;
}
