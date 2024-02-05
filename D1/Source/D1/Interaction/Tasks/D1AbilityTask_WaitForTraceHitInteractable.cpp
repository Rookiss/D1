#include "D1AbilityTask_WaitForTraceHitInteractable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForTraceHitInteractable)

UD1AbilityTask_WaitForTraceHitInteractable::UD1AbilityTask_WaitForTraceHitInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_WaitForTraceHitInteractable* UD1AbilityTask_WaitForTraceHitInteractable::WaitForTraceHitInteractable(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile,
	FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange, float InteractionScanRate, bool bShowDebug)
{
	UD1AbilityTask_WaitForTraceHitInteractable* TaskObject = NewAbilityTask<UD1AbilityTask_WaitForTraceHitInteractable>(OwningAbility);
	TaskObject->TraceProfile = TraceProfile;
	TaskObject->StartLocation = StartLocation;
	TaskObject->InteractionScanRange = InteractionScanRange;
	TaskObject->InteractionScanRate = InteractionScanRate;
	TaskObject->bShowDebug = bShowDebug;
	return TaskObject;
}

void UD1AbilityTask_WaitForTraceHitInteractable::Activate()
{
	SetWaitingOnAvatar();

	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UD1AbilityTask_WaitForTraceHitInteractable::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForTraceHitInteractable::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (AvatarActor == nullptr)
		return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UD1AbilityTask_WaitForInteractable)/*Used for Profiling*/, false);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	
	// AimWitchPlayerController() - Set TraceEnd
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	FVector CameraStart;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraStart, CameraRotation);

	const FVector CameraDirection = CameraRotation.Vector();
	FVector CameraEnd = CameraStart + (CameraDirection * InteractionScanRange);
	// AimWitchPlayerController();

	// TODO
	// 1인칭 시점일때는 카메라의 앞방향을 기준으로 최대 인터렉션 길이만큼 라인트레이스를 하면 되지만
	// 3인칭 시점일때 1인칭하고 똑같이해버리면 캐릭터와 카메라 사이의 거리때문에 캐릭터와 카메라 사이에서 라인트레이스가 멈춰버려 폰이 아무리 아이템 가까이 다가가도 라인트레이스 Hit이 안되는 문제가 발생할 수 있다.
	// 트레이스 시작점을 카메라로 하고 트레이스 길이만 늘리면 캐릭터와 카메라 사이에 있는 오브젝트가 Hit이 될 수 있어 문제가 된다
	// 카메라에서 캐릭터까지의 벡터를 카메라 방향에 투영시켜 그 지점부터 라인트레이스를 하면 어떨까 싶지만 1인칭 시점에서 인터렉션할 수 있는 영역과 3인칭에서 인터렉션할 수 있는 영역이 달라져 문제가 된다.
<<<<<<< HEAD
	// 결국, 라인트레이스 시작점을 캐릭터로 하고, 캐릭터 주변으로 반지름이 최대 인터렉션 길이인 구를 그린다음 카메라의 앞 방향 벡터와 교차하는 지점을 라인트레이스 끝지점으로 정한다.
=======
	// 결국, 라인트레이스 시작점을 캐릭터로 하고, 캐릭터 주변으로 반지름이 최대 인터렉션 길이인 구를 그린다음 카메라의 앞 방향 벡터와 교차하는 지점을 새로운 라인트레이스 끝지점으로 정한다.
>>>>>>> 52468f187caea1631a2acf6da2b94d0a59adb096
	// 이렇게 하면 1인칭 시점에서 인터렉션이 불가능한 오브젝트가 3인칭에서 인터렉션이 가능해지는 문제를 예방할 수 있다.
	
	// ClipCameraRayToAbilityRange()
	FVector CameraToPawn = TraceStart - CameraStart;
	
	float CameraToDot = FVector::DotProduct(CameraToPawn, CameraDirection/*Unit Vector*/);
	
	float DistanceSquared = CameraToPawn.SizeSquared() - (CameraToDot * CameraToDot);
	float RadiusSquared = (InteractionScanRange * InteractionScanRange);

	float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
	float DistanceAlongRay = CameraToDot + DistanceFromCamera;

	CameraEnd = CameraStart + (DistanceAlongRay * CameraDirection);
	// ClipCameraRayToAbilityRange()
	
	FHitResult OutHitResult = FHitResult();
	
	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByProfile(HitResults, TraceStart, TraceEnd, TraceProfile.Name, Params);

	OutHitResult.TraceStart = TraceStart;
	OutHitResult.TraceEnd = TraceEnd;

	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}

	TArray<TScriptInterface<ID1Interactable>> Interactables;
	// Gathering From HitResult

	UpdateInteractableInfos(Interactables);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (OutHitResult.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), TraceStart, OutHitResult.Location, DebugColor, false, InteractionScanRate);
			DrawDebugSphere(GetWorld(), OutHitResult.Location, 5.f, 16, DebugColor, false, InteractionScanRate);
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, InteractionScanRate);
		}
	}
#endif // ENABLE_DRAW_DEBUG
}

void UD1AbilityTask_WaitForTraceHitInteractable::UpdateInteractableInfos(const TArray<TScriptInterface<ID1Interactable>>& Interactables)
{
	
}
