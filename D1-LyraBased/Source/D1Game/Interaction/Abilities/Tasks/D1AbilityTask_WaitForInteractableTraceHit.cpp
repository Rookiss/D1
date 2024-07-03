#include "D1AbilityTask_WaitForInteractableTraceHit.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1InteractionInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForInteractableTraceHit)

UD1AbilityTask_WaitForInteractableTraceHit::UD1AbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForInteractableTraceHit* UD1AbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FD1InteractionQuery InteractionQuery, FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionTraceRange, float InteractionTraceRate, bool bShowDebug)
{
	UD1AbilityTask_WaitForInteractableTraceHit* Task = NewAbilityTask<UD1AbilityTask_WaitForInteractableTraceHit>(OwningAbility);
	Task->InteractionTraceRange = InteractionTraceRange;
	Task->InteractionTraceRate = InteractionTraceRate;
	Task->StartLocation = StartLocation;
	Task->InteractionQuery = InteractionQuery;
	Task->TraceProfile = TraceProfile;
	Task->bShowDebug = bShowDebug;
	return Task;
}

void UD1AbilityTask_WaitForInteractableTraceHit::Activate()
{
	Super::Activate();
	
	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionTraceRate, true);
	}
}

void UD1AbilityTask_WaitForInteractableTraceHit::OnDestroy(bool bInOwnerFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TraceTimerHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForInteractableTraceHit::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (AvatarActor == nullptr)
		return;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(D1AbilityTask_WaitForInteractableTraceHit), false);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(AvatarActor, Params, TraceStart, InteractionTraceRange, TraceEnd);

	FHitResult HitResult;
	LineTrace(TraceStart, TraceEnd, TraceProfile.Name, Params, HitResult);

	TArray<TScriptInterface<ID1Interactable>> Interactables;
	TScriptInterface<ID1Interactable> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		Interactables.AddUnique(InteractableActor);
	}

	TScriptInterface<ID1Interactable> InteractableComponent(HitResult.GetComponent());
	if (InteractableComponent)
	{
		Interactables.AddUnique(InteractableComponent);
	}

	UpdateInteractionInfos(InteractionQuery, Interactables);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = HitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, DebugColor, false, InteractionTraceRate);
			DrawDebugSphere(GetWorld(), HitResult.Location, 5.f, 16, DebugColor, false, InteractionTraceRate);
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, InteractionTraceRate);
		}
	}
#endif // ENABLE_DRAW_DEBUG
}

void UD1AbilityTask_WaitForInteractableTraceHit::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnorePitch) const
{
	if (Ability == nullptr)
		return;
	
	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	check(PlayerController);

	FVector CameraStart;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraStart, CameraRotation);

	const FVector CameraDirection = CameraRotation.Vector();
	FVector CameraEnd = CameraStart + (CameraDirection * MaxRange);
	
	ClipCameraRayToAbilityRange(CameraStart, CameraDirection, TraceStart, MaxRange, CameraEnd);

	FHitResult HitResult;
	LineTrace(CameraStart, CameraEnd, TraceProfile.Name, Params, HitResult);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));
	const FVector AdjustedEnd = bUseTraceResult ? HitResult.Location : CameraEnd;

	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = CameraDirection;
	}

	if (bTraceAffectsAimPitch == false && bUseTraceResult)
	{
		FVector OriginalAimDir = (CameraEnd - TraceStart).GetSafeNormal();

		if (OriginalAimDir.IsZero() == false)
		{
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool UD1AbilityTask_WaitForInteractableTraceHit::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPosition) const
{
	// 캐릭터 주변으로 반지름이 Ability Range인 구를 그린다음 카메라의 앞 방향 벡터와 구가 교차하는 지점을 구한다
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	float DistanceCameraToDot = FVector::DotProduct(CameraToCenter, CameraDirection);
	if (DistanceCameraToDot >= 0)
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DistanceCameraToDot * DistanceCameraToDot);
		float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceDotToSphere = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceCameraToSphere = DistanceCameraToDot + DistanceDotToSphere;
			OutClippedPosition = CameraLocation + (DistanceCameraToSphere * CameraDirection);
			return true;
		}
	}
	return false;
}

void UD1AbilityTask_WaitForInteractableTraceHit::LineTrace(const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams& Params, FHitResult& OutHitResult) const
{
	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);
	
	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}
	else
	{
		OutHitResult = FHitResult();
		OutHitResult.TraceStart = Start;
		OutHitResult.TraceEnd = End;
	}
}

void UD1AbilityTask_WaitForInteractableTraceHit::UpdateInteractionInfos(const FD1InteractionQuery& InteractQuery, const TArray<TScriptInterface<ID1Interactable>>& Interactables)
{
	TArray<FD1InteractionInfo> NewInteractionInfos;

	for (const TScriptInterface<ID1Interactable>& Interactable : Interactables)
	{
		TArray<FD1InteractionInfo> TempInteractionInfos;
		FD1InteractionInfoBuilder InteractionInfoBuilder(Interactable, TempInteractionInfos);
		Interactable->GatherPostInteractionInfos(InteractQuery, InteractionInfoBuilder);

		for (FD1InteractionInfo& InteractionInfo : TempInteractionInfos)
		{
			if (InteractionInfo.InteractionAbilityToGrant)
			{
				FGameplayAbilitySpec* InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InteractionInfo.InteractionAbilityToGrant);
				if (InteractionAbilitySpec)
				{
					if (InteractionAbilitySpec->Ability->CanActivateAbility(InteractionAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
					{
						NewInteractionInfos.Add(InteractionInfo);
					}
				}
			}
		}
	}

	bool bInfosChanged = false;
	if (NewInteractionInfos.Num() == CurrentInteractionInfos.Num())
	{
		NewInteractionInfos.Sort();

		for (int InfoIndex = 0; InfoIndex < NewInteractionInfos.Num(); InfoIndex++)
		{
			const FD1InteractionInfo& NewInfo = NewInteractionInfos[InfoIndex];
			const FD1InteractionInfo& CurrentInfo = CurrentInteractionInfos[InfoIndex];

			if (NewInfo != CurrentInfo)
			{
				bInfosChanged = true;
				break;
			}
		}
	}
	else
	{
		bInfosChanged = true;
	}

	if (bInfosChanged)
	{
		CurrentInteractionInfos = NewInteractionInfos;
		InteractableChanged.Broadcast(CurrentInteractionInfos);
	}
}
