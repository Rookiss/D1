#include "D1AbilityTask_WaitForLineTraceHitInteractables.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForLineTraceHitInteractables)

UD1AbilityTask_WaitForLineTraceHitInteractables::UD1AbilityTask_WaitForLineTraceHitInteractables(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForLineTraceHitInteractables* UD1AbilityTask_WaitForLineTraceHitInteractables::WaitForLineTraceHitInteractables(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile, float InteractionScanRange, float InteractionScanRate)
{
	UD1AbilityTask_WaitForLineTraceHitInteractables* Task = NewAbilityTask<UD1AbilityTask_WaitForLineTraceHitInteractables>(OwningAbility);
	Task->TraceProfile = TraceProfile;
	Task->InteractionScanRange = InteractionScanRange;
	Task->InteractionScanRate = InteractionScanRate;
	return Task;
}

void UD1AbilityTask_WaitForLineTraceHitInteractables::Activate()
{
	SetWaitingOnAvatar();

	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UD1AbilityTask_WaitForLineTraceHitInteractables::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForLineTraceHitInteractables::PerformTrace()
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
	
	TScriptInterface<ID1Interactable> Interactable(HitResult.GetActor());
	HandleInteractableInfo(Interactable);

#if ENABLE_DRAW_DEBUG
	FColor DebugColor = HitResult.bBlockingHit ? FColor::Red : FColor::Green;
	if (HitResult.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, DebugColor, false, InteractionScanRate);
		DrawDebugSphere(GetWorld(), HitResult.Location, 5, 16, DebugColor, false, InteractionScanRate);
	}
	else
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, InteractionScanRate);
	}
#endif // ENABLE_DRAW_DEBUG
}

void UD1AbilityTask_WaitForLineTraceHitInteractables::HandleInteractableInfo(const TScriptInterface<ID1Interactable>& Interactable)
{
	FD1InteractionInfo NewInfo;

	if (Interactable)
	{
		FD1InteractionInfo PendingInfo = Interactable->GetInteractionInfo();

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
