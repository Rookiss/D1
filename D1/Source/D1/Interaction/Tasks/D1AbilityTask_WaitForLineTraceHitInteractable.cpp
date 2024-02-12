#include "D1AbilityTask_WaitForLineTraceHitInteractable.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"
#include "Physics/D1CollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForLineTraceHitInteractable)

UD1AbilityTask_WaitForLineTraceHitInteractable::UD1AbilityTask_WaitForLineTraceHitInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForLineTraceHitInteractable* UD1AbilityTask_WaitForLineTraceHitInteractable::WaitForLineTraceHitInteractable(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile, float InteractionScanRange, float InteractionScanRate, bool bShowDebug)
{
	UD1AbilityTask_WaitForLineTraceHitInteractable* Task = NewAbilityTask<UD1AbilityTask_WaitForLineTraceHitInteractable>(OwningAbility);
	Task->TraceProfile = TraceProfile;
	Task->InteractionScanRange = InteractionScanRange;
	Task->InteractionScanRate = InteractionScanRate;
	Task->bShowDebug = bShowDebug;
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
	
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (PC == nullptr)
		return;
	
	FVector CameraStart;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraStart, CameraRotation);
		
	TraceStart = CameraStart;
	TraceEnd = CameraStart + (CameraRotation.Vector() * InteractionScanRange);
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, D1_TraceChannel_Interaction, Params);

	bool bShouldBroadcast = true;
	
	if (HitResult.bBlockingHit)
	{
		if (CurrentTargetDataHandle.Num() > 0)
		{
			const AActor* OldTarget = CurrentTargetDataHandle.Get(0)->GetHitResult()->GetActor();
			if (OldTarget == HitResult.GetActor())
			{
				bShouldBroadcast = false;
			}
		}
	}
	else if (CurrentTargetDataHandle.Num() == 0)
	{
		bShouldBroadcast = false;
	}

	if (bShouldBroadcast)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(TargetData);
		
		CurrentTargetDataHandle = TargetDataHandle;
		InteractableFocusChanged.Broadcast(CurrentTargetDataHandle);
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
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
	}
#endif // ENABLE_DRAW_DEBUG
}
