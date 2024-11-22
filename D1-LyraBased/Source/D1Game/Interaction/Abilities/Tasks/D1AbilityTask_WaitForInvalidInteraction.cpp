#include "D1AbilityTask_WaitForInvalidInteraction.h"

#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForInvalidInteraction)

UD1AbilityTask_WaitForInvalidInteraction::UD1AbilityTask_WaitForInvalidInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForInvalidInteraction* UD1AbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(UGameplayAbility* OwningAbility, float AcceptanceAngle, float AcceptanceDistance)
{
	UD1AbilityTask_WaitForInvalidInteraction* Task = NewAbilityTask<UD1AbilityTask_WaitForInvalidInteraction>(OwningAbility);
	Task->AcceptanceAngle = AcceptanceAngle;
	Task->AcceptanceDistance = AcceptanceDistance;
	return Task;
}

void UD1AbilityTask_WaitForInvalidInteraction::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	CachedCharacterForward2D = GetAvatarActor() ? GetAvatarActor()->GetActorForwardVector().GetSafeNormal2D() : FVector::ZeroVector;
	CachedCharacterLocation = GetAvatarActor() ? GetAvatarActor()->GetActorLocation() : FVector::ZeroVector;

	GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &ThisClass::PerformCheck, 0.05f, true);
}

void UD1AbilityTask_WaitForInvalidInteraction::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForInvalidInteraction::PerformCheck()
{
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(Ability->GetCurrentActorInfo()->AvatarActor.Get());
	UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement();

	if (LyraCharacter && CharacterMovement)
	{
		bool bValidAngle2D = CalculateAngle2D() <= AcceptanceAngle;
		bool bValidDistanceXY = FVector::DistSquared2D(CachedCharacterLocation, LyraCharacter->GetActorLocation()) <= (AcceptanceDistance * AcceptanceDistance);
		bool bValidDistanceZ = FMath::Abs(CachedCharacterLocation.Z - LyraCharacter->GetActorLocation().Z) <= (AcceptanceDistance + FMath::Abs(CharacterMovement->GetCrouchedHalfHeight() - LyraCharacter->BaseUnscaledCapsuleHalfHeight));

		if (bValidAngle2D && bValidDistanceXY && bValidDistanceZ)
			return;
	}

	OnInvalidInteraction.Broadcast();
	EndTask();
}

float UD1AbilityTask_WaitForInvalidInteraction::CalculateAngle2D() const
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	
	if (AvatarActor && PlayerController)
	{
		FVector CharacterForward2D = AvatarActor->GetActorForwardVector().GetSafeNormal2D();
		return UKismetMathLibrary::DegAcos(CachedCharacterForward2D.Dot(CharacterForward2D));
	}
	
	return 0.f;
}
