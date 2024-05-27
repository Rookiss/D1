#include "D1AnimNotifyState_PerformTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "KismetTraceUtils.h"
#include "Components/BoxComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_PerformTrace)

UD1AnimNotifyState_PerformTrace::UD1AnimNotifyState_PerformTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UD1AnimNotifyState_PerformTrace::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;
	
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>())
		{
			WeaponActor = EquipManager->GetEquippedWeapon(WeaponHandType);
			if (WeaponActor.IsValid() == false)
				return;
			
			PreviousTraceTransform = WeaponActor->WeaponMeshComponent->GetComponentTransform();
			PreviousDebugTransform = WeaponActor->TraceDebugCollision->GetComponentTransform();
			PreviousSocketTransform = WeaponActor->WeaponMeshComponent->GetSocketTransform(TraceParams.TraceSocketName);

#if UE_EDITOR
	check(WeaponActor->WeaponMeshComponent->DoesSocketExist(TraceParams.TraceSocketName));
#endif
		}
	}

	HitActors.Empty();
}

void UD1AnimNotifyState_PerformTrace::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);	

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;
	
	if (WeaponActor.IsValid() == false)
		return;
	
	PerformTrace(MeshComponent);
}

void UD1AnimNotifyState_PerformTrace::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
	
	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;
	
	if (WeaponActor.IsValid() == false)
		return;

	PerformTrace(MeshComponent);
}

void UD1AnimNotifyState_PerformTrace::PerformTrace(USkeletalMeshComponent* MeshComponent)
{
	const FTransform& CurrentSocketTransform = WeaponActor->WeaponMeshComponent->GetSocketTransform(TraceParams.TraceSocketName);
	float Distance = (PreviousSocketTransform.GetLocation() - CurrentSocketTransform.GetLocation()).Length();
		
	int SubStepCount = FMath::CeilToInt(Distance / TraceParams.TargetDistance);
	if (SubStepCount <= 0)
		return;
	
	const float SubstepRatio = 1.f / SubStepCount;

	const FTransform& CurrentTraceTransform = WeaponActor->WeaponMeshComponent->GetComponentTransform();
	const FTransform& CurrentDebugTransform = WeaponActor->TraceDebugCollision->GetComponentTransform();

	TArray<FHitResult> FinalHitResults;
	
	for (int32 i = 0; i < SubStepCount; i++)
	{
		const FTransform& StartTraceTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
		const FTransform& EndTraceTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
		const FTransform& AverageTraceTransform = UKismetMathLibrary::TLerp(StartTraceTransform, EndTraceTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);

		FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
		Params.bReturnPhysicalMaterial = true;
		
		TArray<AActor*> IgnoredActors = { WeaponActor.Get(), WeaponActor->GetOwner() };
		Params.AddIgnoredActors(IgnoredActors);

		TArray<FHitResult> HitResults;
		MeshComponent->GetWorld()->ComponentSweepMulti(HitResults, WeaponActor->WeaponMeshComponent, StartTraceTransform.GetLocation(), EndTraceTransform.GetLocation(), AverageTraceTransform.GetRotation(), Params);

		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActors.Contains(HitActor) == false)
			{
				HitActors.Add(HitActor);
				FinalHitResults.Add(HitResult);
			}
		}

#if UE_EDITOR
		if (TraceDebugParams.bDrawDebugShape)
		{
			FColor Color = (HitResults.Num() > 0) ? TraceDebugParams.HitColor : TraceDebugParams.TraceColor;
			
			const FTransform& StartDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
			const FTransform& EndDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
			const FTransform& AverageDebugTransform = UKismetMathLibrary::TLerp(StartDebugTransform, EndDebugTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);
			
			DrawDebugSweptBox(MeshComponent->GetWorld(), StartDebugTransform.GetLocation(), EndDebugTransform.GetLocation(), AverageDebugTransform.GetRotation().Rotator(), WeaponActor->TraceDebugCollision->GetScaledBoxExtent(), Color, false, 2.f);
		}
#endif
	}
	
	PreviousTraceTransform = CurrentTraceTransform;
	PreviousDebugTransform = CurrentDebugTransform;
	PreviousSocketTransform = CurrentSocketTransform;
	
	if (FinalHitResults.Num() > 0)
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		
		for (const FHitResult& HitResult : FinalHitResults)
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = HitResult;
			TargetDataHandle.Add(NewTargetData);
		}
		
		FGameplayEventData EventData;
		EventData.TargetData = TargetDataHandle;

		if (EventTag.IsValid())
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComponent->GetOwner(), EventTag, EventData);
		}
	}
}
