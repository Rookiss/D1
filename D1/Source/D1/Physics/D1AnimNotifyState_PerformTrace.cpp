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
	, TargetTickInterval(0)
	, bIsFirstTrace(false)
	, LastTickTime(0)
{
	
}

void UD1AnimNotifyState_PerformTrace::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	if (ExecuteNetRole != MeshComponent->GetOwnerRole())
		return;
	
	if (AD1Player* PlayerCharacter = Cast<AD1Player>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = PlayerCharacter->EquipManagerComponent)
		{
			const TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			WeaponActor = Entries[(int32)EquipManager->ConvertToEquipmentSlotType(WeaponHandType)].SpawnedWeaponActor;
			TargetTickInterval = 1.f / TargetFPS;
			bIsFirstTrace = true;
			PreviousTransform = WeaponActor->TraceCollision->GetComponentTransform();
		}
	}
}

void UD1AnimNotifyState_PerformTrace::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);	

	if (ExecuteNetRole != MeshComponent->GetOwnerRole())
		return;
	
	if (WeaponActor == nullptr)
		return;

	LastTickTime = FApp::GetCurrentTime();
	PerformTrace(MeshComponent, FrameDeltaTime);
}

void UD1AnimNotifyState_PerformTrace::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
	
	if (ExecuteNetRole != MeshComponent->GetOwnerRole())
		return;
	
	if (WeaponActor == nullptr)
		return;

	PerformTrace(MeshComponent, FApp::GetCurrentTime() - LastTickTime);
}

void UD1AnimNotifyState_PerformTrace::PerformTrace(USkeletalMeshComponent* MeshComponent, float DeltaTime)
{
	const FTransform& CurrentTransform = WeaponActor->TraceCollision->GetComponentTransform();
	TArray<FHitResult> HitResults;
	
	int SubSteps = FMath::CeilToInt(DeltaTime / TargetTickInterval);
	SubSteps = FMath::Clamp(SubSteps, 1, 20);

	const float SubstepRatio = 1.f / SubSteps;

	for (int32 i = 0; i < SubSteps; i++)
	{
		const FTransform& StartTransform = UKismetMathLibrary::TLerp(PreviousTransform, CurrentTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
		const FTransform& EndTransform = UKismetMathLibrary::TLerp(PreviousTransform, CurrentTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
		const FTransform& AverageTransform = UKismetMathLibrary::TLerp(StartTransform, EndTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);

		FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
		Params.bReturnPhysicalMaterial = true;
		TArray<AActor*> IgnoredActors = { WeaponActor, WeaponActor->GetOwner() };
		Params.AddIgnoredActors(IgnoredActors);

		MeshComponent->GetWorld()->ComponentSweepMulti(HitResults, WeaponActor->TraceCollision, StartTransform.GetLocation(), EndTransform.GetLocation(), AverageTransform.GetRotation(), Params);

		if (bDrawDebugShape)
		{
			FColor Color = (HitResults.Num() > 0) ? FColor::Green : FColor::Red;
			DrawDebugSweptBox(MeshComponent->GetWorld(), StartTransform.GetLocation(), EndTransform.GetLocation(), AverageTransform.GetRotation().Rotator(), WeaponActor->TraceCollision->GetScaledBoxExtent(), Color, true, DeltaTime * 2);
		}
	}
	
	PreviousTransform = CurrentTransform;

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
