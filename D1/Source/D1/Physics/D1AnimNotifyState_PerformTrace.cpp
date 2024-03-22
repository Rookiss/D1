#include "D1AnimNotifyState_PerformTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "KismetTraceUtils.h"
#include "Character/D1Player.h"
#include "Components/BoxComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_PerformTrace)

UD1AnimNotifyState_PerformTrace::UD1AnimNotifyState_PerformTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShouldFireInEditor = false;
}

void UD1AnimNotifyState_PerformTrace::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;
	
	if (AD1Player* PlayerCharacter = Cast<AD1Player>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = PlayerCharacter->EquipManagerComponent)
		{
			const TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			WeaponActor = Entries[(int32)EquipManager->ConvertToEquipmentSlotType(WeaponHandType)].SpawnedWeaponActor;

			TargetDeltaTime = 1.f / TargetFPS;
			
			PreviousTraceTransform = WeaponActor->WeaponMeshComponent->GetComponentTransform();
			PreviousDebugTransform = WeaponActor->TraceDebugCollision->GetComponentTransform();
			PreviousSocketTransform = WeaponActor->WeaponMeshComponent->GetSocketTransform(TraceSocketName);
		}
	}
}

void UD1AnimNotifyState_PerformTrace::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);	

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;
	
	if (WeaponActor == nullptr)
		return;

	LastTickTime = FApp::GetCurrentTime();
	PerformTrace(MeshComponent, FrameDeltaTime);
}

void UD1AnimNotifyState_PerformTrace::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
	
	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;
	
	if (WeaponActor == nullptr)
		return;

	PerformTrace(MeshComponent, FApp::GetCurrentTime() - LastTickTime);
}

void UD1AnimNotifyState_PerformTrace::PerformTrace(USkeletalMeshComponent* MeshComponent, float DeltaTime)
{
	const FTransform& CurrentTraceTransform = WeaponActor->WeaponMeshComponent->GetComponentTransform();
	const FTransform& CurrentDebugTransform = WeaponActor->TraceDebugCollision->GetComponentTransform();
	
	int SubSteps = 1;

	if (TraceType == ETraceType::Distance)
	{
		const FTransform& CurrentSocketTransform = WeaponActor->WeaponMeshComponent->GetSocketTransform(TraceSocketName);
		Distance = (PreviousSocketTransform.GetLocation() - CurrentSocketTransform.GetLocation()).Length();
		PreviousSocketTransform = CurrentSocketTransform;
		
		SubSteps = FMath::CeilToInt(Distance / TargetDistance);
	}
	else if (TraceType == ETraceType::Frame)
	{
		SubSteps = FMath::CeilToInt(DeltaTime / TargetDeltaTime);
	}

	if (SubSteps <= 0)
		return;

	TArray<FHitResult> HitResults;
	const float SubstepRatio = 1.f / SubSteps;
	
	for (int32 i = 0; i < SubSteps; i++)
	{
		const FTransform& StartTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
		const FTransform& EndTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
		const FTransform& AverageTransform = UKismetMathLibrary::TLerp(StartTransform, EndTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);

		FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
		Params.bReturnPhysicalMaterial = true;
		TArray<AActor*> IgnoredActors = { WeaponActor, WeaponActor->GetOwner() };
		Params.AddIgnoredActors(IgnoredActors);
		
		MeshComponent->GetWorld()->ComponentSweepMulti(HitResults, WeaponActor->WeaponMeshComponent, StartTransform.GetLocation(), EndTransform.GetLocation(), AverageTransform.GetRotation(), Params);
		
		if (bDrawDebugShape)
		{
			FColor Color = (HitResults.Num() > 0) ? HitColor : TraceColor;
			
			const FTransform& StartDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
			const FTransform& EndDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
			const FTransform& AverageDebugTransform = UKismetMathLibrary::TLerp(StartDebugTransform, EndDebugTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);
			
			DrawDebugSweptBox(MeshComponent->GetWorld(), StartDebugTransform.GetLocation(), EndDebugTransform.GetLocation(), AverageDebugTransform.GetRotation().Rotator(), WeaponActor->TraceDebugCollision->GetScaledBoxExtent(), Color, false, 2.f);
		}
	}
	
	PreviousTraceTransform = CurrentTraceTransform;
	PreviousDebugTransform = CurrentDebugTransform;

	if (HitResults.Num() > 0)
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		
		for (const FHitResult& HitResult : HitResults)
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
