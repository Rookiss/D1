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
	Super::Activate();
	
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
	if (Ability == nullptr)
		return;
	
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (AvatarActor == nullptr)
		return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UD1AbilityTask_WaitForInteractable)/*Used for Profiling*/, false);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart;
	FVector TraceEnd;
	
	if (APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
	{
		FVector CameraStart;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraStart, CameraRotation);
		
		TraceStart = CameraStart;
		TraceEnd = CameraStart + (CameraRotation.Vector() * InteractionScanRange);
	}
	else
	{
		TraceStart = AvatarActor->GetActorLocation();
		TraceEnd = TraceStart + (AvatarActor->GetActorForwardVector() * InteractionScanRange);
	}
	
	FHitResult HitResult;
	LineTrace(HitResult, TraceStart, TraceEnd, TraceProfile.Name, Params);

	if (HitResult.bBlockingHit == false)
	{
		if (TargetDataHandle.Num() > 0 && TargetDataHandle.Get(0)->GetHitResult()->GetActor())
		{
			LostInteractable.Broadcast(TargetDataHandle);

			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			TargetDataHandle = FGameplayAbilityTargetDataHandle(TargetData);
		}
	}
	else
	{
		bool bFoundNewTarget = true;
		
		if (TargetDataHandle.Num() > 0)
		{
			const AActor* OldTarget = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
			if (OldTarget == HitResult.GetActor())
			{
				bFoundNewTarget = false;
			}
			else if (OldTarget)
			{
				LostInteractable.Broadcast(TargetDataHandle);
			}
		}

		if (bFoundNewTarget)
		{
			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			TargetDataHandle = FGameplayAbilityTargetDataHandle(TargetData);
			FoundNewInteractable.Broadcast(TargetDataHandle);
		}
	}

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

void UD1AbilityTask_WaitForLineTraceHitInteractable::LineTrace(FHitResult& OutHitResult, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	OutHitResult = FHitResult();
	
	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);
	
	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}
}
