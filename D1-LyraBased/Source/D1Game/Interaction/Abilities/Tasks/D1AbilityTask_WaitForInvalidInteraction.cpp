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

	CachedAngle2D = CalculateAngle2D();
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
		bool bValidAngle2D = FMath::Abs(CachedAngle2D - CalculateAngle2D()) <= AcceptanceAngle;
		bool bValidDistanceXY = FVector::DistSquared2D(CachedCharacterLocation, LyraCharacter->GetActorLocation()) <= (AcceptanceDistance * AcceptanceDistance);
		bool bValidDistanceZ = FMath::Abs(CachedCharacterLocation.Z - LyraCharacter->GetActorLocation().Z) <= (AcceptanceDistance + FMath::Abs(CharacterMovement->GetCrouchedHalfHeight() - LyraCharacter->BaseUnscaledCapsuleHalfHeight));

		if (bValidAngle2D && bValidDistanceXY && bValidDistanceZ)
			return;
	}

	InvalidInteraction.Broadcast();
	EndTask();
}

float UD1AbilityTask_WaitForInvalidInteraction::CalculateAngle2D() const
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	
	if (AvatarActor && PlayerController)
	{
		FVector CameraStart;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraStart, CameraRotation);

		FVector CharacterToTarget = (InteractionLocation - AvatarActor->GetActorLocation()).GetSafeNormal2D();
		FVector CameraDirection = CameraRotation.Vector().GetSafeNormal2D();
		return UKismetMathLibrary::DegAcos(CharacterToTarget.Dot(CameraDirection));
	}
	
	return 0.f;
}
