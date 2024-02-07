#include "D1AbilityTask_WaitForLineTraceHitInteractable.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForLineTraceHitInteractable)

UD1AbilityTask_WaitForLineTraceHitInteractable::UD1AbilityTask_WaitForLineTraceHitInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForLineTraceHitInteractable* UD1AbilityTask_WaitForLineTraceHitInteractable::WaitForLineTraceHitInteractable(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile, float InteractionScanRange, float InteractionScanRate)
{
	UD1AbilityTask_WaitForLineTraceHitInteractable* Task = NewAbilityTask<UD1AbilityTask_WaitForLineTraceHitInteractable>(OwningAbility);
	Task->TraceProfile = TraceProfile;
	Task->InteractionScanRange = InteractionScanRange;
	Task->InteractionScanRate = InteractionScanRate;
	return Task;
}

void UD1AbilityTask_WaitForLineTraceHitInteractable::Activate()
{
	SetWaitingOnAvatar();

	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UD1AbilityTask_WaitForLineTraceHitInteractable::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForLineTraceHitInteractable::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (AvatarActor == nullptr || Ability == nullptr)
		return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UD1AbilityTask_WaitForInteractable)/*Used for Profiling*/, false);
	Params.AddIgnoredActors(ActorsToIgnore);
	
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	check(PC);

	FVector CameraStart;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraStart, CameraRotation);

	const FVector CameraDirection = CameraRotation.Vector();

	FVector TraceStart = CameraStart;
	FVector TraceEnd = CameraStart + (CameraDirection * InteractionScanRange);
		
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByProfile(HitResult, TraceStart, TraceEnd, TraceProfile.Name, Params);
	
	HandleInteractableInfo(HitResult.GetActor());

#if ENABLE_DRAW_DEBUG
	if (HitResult.bBlockingHit && HitResult.GetActor()->Implements<UD1Interactable>())
	{
		DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, FColor::Red, false, InteractionScanRate);
		DrawDebugSphere(GetWorld(), HitResult.Location, 5, 16, FColor::Red, false, InteractionScanRate);
	}
	else
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, InteractionScanRate);
	}
#endif // ENABLE_DRAW_DEBUG
}

void UD1AbilityTask_WaitForLineTraceHitInteractable::HandleInteractableInfo(const TScriptInterface<ID1Interactable>& Interactable)
{
	FD1InteractionInfo NewInfo;

	if (Interactable)
	{
		const FD1InteractionInfo& PendingInfo = Interactable->GetInteractionInfo();

		if (PendingInfo.InteractionAbilityToGrant)
		{
			if (FGameplayAbilitySpec* InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(PendingInfo.InteractionAbilityToGrant))
			{
				if (InteractionAbilitySpec->Ability->CanActivateAbility(InteractionAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
				{
					NewInfo = PendingInfo;
					NewInfo.InteractionAbilityHandle = InteractionAbilitySpec->Handle;
				}
			}
		}
	}

	if (NewInfo != LatestInfo)
	{
		LatestInfo = NewInfo;
		OnInteractableChanged.Broadcast(LatestInfo);
	}
}
