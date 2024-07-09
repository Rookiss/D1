#include "D1AbilityTask_WaitForInvalidInteraction.h"

#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForInvalidInteraction)

UD1AbilityTask_WaitForInvalidInteraction::UD1AbilityTask_WaitForInvalidInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForInvalidInteraction* UD1AbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(UGameplayAbility* OwningAbility, FVector InteractionLocation, float AcceptanceAngle, float AcceptanceDistance)
{
	UD1AbilityTask_WaitForInvalidInteraction* Task = NewAbilityTask<UD1AbilityTask_WaitForInvalidInteraction>(OwningAbility);
	Task->InteractionLocation = InteractionLocation;
	Task->AcceptanceAngle = AcceptanceAngle;
	Task->AcceptanceDistance = AcceptanceDistance;
	return Task;
}

void UD1AbilityTask_WaitForInvalidInteraction::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	CachedAngle = CalculateAngle();
	CachedCharacterLocation = GetAvatarActor() ? GetAvatarActor()->GetActorLocation() : FVector::ZeroVector;

	GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &ThisClass::PerformCheck, 0.1f, true);
}

void UD1AbilityTask_WaitForInvalidInteraction::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForInvalidInteraction::PerformCheck()
{
	ALyraCharacter* AvatarActor = Cast<ALyraCharacter>(Ability->GetCurrentActorInfo()->AvatarActor.Get());
	UCharacterMovementComponent* CharacterMovement = AvatarActor->GetCharacterMovement();
	APlayerController* AvatarController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	
	if (AvatarActor && CharacterMovement && AvatarController)
	{
		bool bValidAngle = FMath::Abs(CachedAngle - CalculateAngle()) <= AcceptanceAngle;
		bool bValidDistanceXY = FVector::DistSquared2D(CachedCharacterLocation, AvatarActor->GetActorLocation()) <= (AcceptanceDistance * AcceptanceDistance);
		bool bValidDistanceZ = FMath::Abs(CachedCharacterLocation.Z - AvatarActor->GetActorLocation().Z) <= (AcceptanceDistance + FMath::Abs(CharacterMovement->GetCrouchedHalfHeight() - AvatarActor->BaseUnscaledCapsuleHalfHeight));
		
		if (bValidAngle && bValidDistanceXY && bValidDistanceZ)
			return;
	}

	InvalidInteraction.Broadcast();
	EndTask();
}

float UD1AbilityTask_WaitForInvalidInteraction::CalculateAngle() const
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	APlayerController* AvatarController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	
	if (AvatarActor && AvatarController)
	{
		FVector CameraStart;
		FRotator CameraRotation;
		AvatarController->GetPlayerViewPoint(CameraStart, CameraRotation);

		FVector CameraToTarget = (InteractionLocation - CameraStart).GetSafeNormal2D();
		FVector CameraDirection = CameraRotation.Vector().GetSafeNormal2D();
		
		return UKismetMathLibrary::DegAcos(CameraToTarget.Dot(CameraDirection));
	}
	return 0.f;
}
