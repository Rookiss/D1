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
	
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 88.0f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionProfileName("PhysicsActor");
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.0f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 150.f;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void AD1Character::InitAbilitySystem()
{
	
}

void AD1Character::ApplyAbilitySystemData(const FName& DataName)
{
	if (HasAuthority() == false)
		return;
	
	if (const UD1AbilitySystemData* Data = UD1AssetManager::GetAssetByName<UD1AbilitySystemData>(DataName))
	{
		Data->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
	}
}

UAbilitySystemComponent* AD1Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
