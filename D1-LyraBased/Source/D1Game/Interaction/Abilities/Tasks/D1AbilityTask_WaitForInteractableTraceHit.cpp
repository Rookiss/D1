#include "D1AbilityTask_WaitForInteractableTraceHit.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1InteractionInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForInteractableTraceHit)

UD1AbilityTask_WaitForInteractableTraceHit::UD1AbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForInteractableTraceHit* UD1AbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FD1InteractionQuery InteractionQuery, ECollisionChannel TraceChannel, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionTraceRange, float InteractionTraceRate, bool bShowDebug)
{
	UD1AbilityTask_WaitForInteractableTraceHit* Task = NewAbilityTask<UD1AbilityTask_WaitForInteractableTraceHit>(OwningAbility);
	Task->InteractionTraceRange = InteractionTraceRange;
	Task->InteractionTraceRate = InteractionTraceRate;
	Task->StartLocation = StartLocation;
	Task->InteractionQuery = InteractionQuery;
	Task->TraceChannel = TraceChannel;
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
	AvatarActor->GetAttachedActors(ActorsToIgnore, false, true);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(D1AbilityTask_WaitForInteractableTraceHit), false);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(AvatarActor, Params, TraceStart, InteractionTraceRange, TraceEnd);

	FHitResult HitResult;
	LineTrace(TraceStart, TraceEnd, Params, HitResult);

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
	if (PlayerController == nullptr)
		return;

	FVector CameraStart;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraStart, CameraRotation);

	const FVector CameraDirection = CameraRotation.Vector();
	FVector CameraEnd = CameraStart + (CameraDirection * MaxRange);

	// 카메라 방향의 Ray를 플레이어 위치 기준의 인터렉션 가능 범위(Sphere) 이내로 제한한다
	ClipCameraRayToAbilityRange(CameraStart, CameraDirection, TraceStart, MaxRange, CameraEnd);

	FHitResult HitResult;
	LineTrace(CameraStart, CameraEnd, Params, HitResult);

	// 1. Hit된 물체가 인터렉션 가능 범위(Sphere) 이내라면, Hit 위치를 TraceEnd 위치로 정한다.
	// 2. Hit된 물체가 없거나 Hit된 물체가 인터렉션 가능 범위(Sphere)를 벗어 났다면, Hit 위치를 무시하고 CameraEnd를 TraceEnd로 정한다.
	// - 이후에 플레이어와 CameraEnd 사이의 물체를 체크한다.
	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));
	const FVector AdjustedEnd = bUseTraceResult ? HitResult.Location : CameraEnd;

	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = CameraDirection;
	}

	// TraceEnd를 최대 인터렉션 가능 위치(Sphere의 표면)까지 확장한다.
	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool UD1AbilityTask_WaitForInteractableTraceHit::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPosition) const
{
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

void UD1AbilityTask_WaitForInteractableTraceHit::LineTrace(const FVector& Start, const FVector& End, const FCollisionQueryParams& Params, FHitResult& OutHitResult) const
{
	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, TraceChannel, Params);
	
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
			if (InteractionInfo.AbilityToGrant)
			{
				FGameplayAbilitySpec* InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InteractionInfo.AbilityToGrant);
				if (InteractionAbilitySpec)
				{
					if (Interactable->CanInteraction(InteractionQuery) && InteractionAbilitySpec->Ability->CanActivateAbility(InteractionAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
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
		HighlightInteractables(CurrentInteractionInfos, false);
		CurrentInteractionInfos = NewInteractionInfos;
		HighlightInteractables(CurrentInteractionInfos, true);
		
		InteractableChanged.Broadcast(CurrentInteractionInfos);
	}
}

void UD1AbilityTask_WaitForInteractableTraceHit::HighlightInteractables(const TArray<FD1InteractionInfo>& InteractionInfos, bool bShouldHighlight)
{
	TArray<UMeshComponent*> MeshComponents;
	for (const FD1InteractionInfo& InteractionInfo : InteractionInfos)
	{
		if (ID1Interactable* Interactable = InteractionInfo.Interactable.GetInterface())
		{
			Interactable->GetMeshComponents(MeshComponents);
		}
	}

	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->SetRenderCustomDepth(bShouldHighlight);
	}
}
