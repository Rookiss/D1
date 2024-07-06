#include "D1AbilityTask_WaitForInvalidInteraction.h"

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
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	APlayerController* AvatarController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	
	if (AvatarActor && AvatarController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%f"), FMath::Abs(CachedAngle - CalculateAngle())));
		
		bool bValidAngle = FMath::Abs(CachedAngle - CalculateAngle()) <= AcceptanceAngle;
		bool bValidDistance = FVector::DistSquared(CachedCharacterLocation, AvatarActor->GetActorLocation()) <= AcceptanceDistance * AcceptanceDistance;
		
		if (bValidDistance && bValidAngle)
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

		FVector CameraToTarget = (InteractionLocation - CameraStart).GetSafeNormal();
		FVector CameraDirection = CameraRotation.Vector();
		
		return UKismetMathLibrary::DegAcos(CameraToTarget.Dot(CameraDirection));
	}
	return 0.f;
}
