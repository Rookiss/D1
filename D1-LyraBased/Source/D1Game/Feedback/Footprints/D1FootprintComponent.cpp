#include "D1FootprintComponent.h"

#include "D1FootprintActor.h"
#include "D1FootprintStyle.h"
#include "Character/LyraCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1FootprintComponent)

UD1FootprintComponent::UD1FootprintComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1FootprintComponent::AnimMotionEffect_Implementation(const FName Bone, const FGameplayTag MotionEffect, USceneComponent* StaticMeshComponent,
															const FVector LocationOffset, const FRotator RotationOffset, const UAnimSequenceBase* AnimationSequence,
															const bool bHitSuccess, const FHitResult HitResult, FGameplayTagContainer Contexts, FVector VFXScale, float AudioVolume, float AudioPitch)
{
	if (bEnableFootprintEffect == false || bHitSuccess == false)
		return;

	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 3.f, 12, FColor::Red, false, 5.f);

	ALyraCharacter* LyraCharacter = GetPawn<ALyraCharacter>();
	if (LyraCharacter == nullptr)
		return;

	USkeletalMeshComponent* CharacterMeshComponent = LyraCharacter->GetMesh();
	
	TSubclassOf<AD1FootprintActor> FootprintClassToUse = DetermineClass(Contexts);
	if (FootprintClassToUse == nullptr)
		return;

	FPooledFootprintList& FootprintPool = PooledFootprintMap.FindOrAdd(FootprintClassToUse);

	AD1FootprintActor* ActorToUse;
	if (FootprintPool.Footprints.Num() > 0)
	{
		ActorToUse = FootprintPool.Footprints.Pop();
	}
	else
	{
		ActorToUse = GetWorld()->SpawnActor<AD1FootprintActor>(FootprintClassToUse);
	}

	check(ActorToUse);

	LiveFootprints.Emplace(ActorToUse, &FootprintPool, GetWorld()->GetTimeSeconds() + FootprintLifeSpan);

	if (GetWorld()->GetTimerManager().IsTimerActive(ReleaseTimerHandle) == false)
	{
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &ThisClass::ReleaseNextFootprints, FootprintLifeSpan);
	}
	
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
	bool bIsRightSide = (Bone == FootprintRightFootBone);
	ActorToUse->InitializeActor(bIsRightSide, SurfaceType);
	
	FName CurrentSocketName = FName(*UKismetStringLibrary::Concat_StrStr(Bone.ToString(), TEXT("_Socket")));
	FTransform FootprintSocketTransform = CharacterMeshComponent->GetSocketTransform(CurrentSocketName, RTS_World);

	FVector SocketForward = UKismetMathLibrary::TransformDirection(FootprintSocketTransform, FVector(1.f, 0.f, 0.f));

	FVector FloorUp = HitResult.Normal;
	FVector FloorRight = FVector::CrossProduct(FloorUp, SocketForward).GetSafeNormal();
	FVector FloorForward = FVector::CrossProduct(FloorRight, FloorUp);
	
	FQuat FootprintQuat = UKismetMathLibrary::MakeRotationFromAxes(FloorForward, FloorRight, FloorUp).Quaternion();

	FootprintSocketTransform.SetLocation(HitResult.ImpactPoint);
	FootprintSocketTransform.SetRotation(FootprintQuat);
	ActorToUse->SetActorTransform(FootprintSocketTransform);
}

void UD1FootprintComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
	
	for (const FLiveFootprintEntry& LiveFootprint : LiveFootprints)
	{
		AD1FootprintActor* FootprintActor = LiveFootprint.FootprintActor;
		if (IsValid(FootprintActor))
		{
			FootprintActor->SetLifeSpan(10.f);
		}
	}
	LiveFootprints.Empty();

	for (const auto& Pair : PooledFootprintMap)
	{
		for (AD1FootprintActor* FootprintActor : Pair.Value.Footprints)
		{
			if (IsValid(FootprintActor))
			{
				FootprintActor->Destroy();
			}
		}
	}
	PooledFootprintMap.Empty();
	
	Super::EndPlay(EndPlayReason);
}

void UD1FootprintComponent::ReleaseNextFootprints()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	int32 NumReleased = 0;
	for (const FLiveFootprintEntry& LiveFootprint : LiveFootprints)
	{
		if (CurrentTime >= LiveFootprint.ReleaseTime)
		{
			NumReleased++;
			if (ensure(LiveFootprint.FootprintActor))
			{
				if (ensure(LiveFootprint.PoolList))
				{
					LiveFootprint.PoolList->Footprints.Push(LiveFootprint.FootprintActor);
				}
				else
				{
					LiveFootprint.FootprintActor->Destroy();
				}
			}
		}
		else
		{
			break;
		}
	}

	LiveFootprints.RemoveAt(0, NumReleased);

	if (LiveFootprints.Num() > 0)
	{
		const float TimeUntilNextRelease = LiveFootprints[0].ReleaseTime - CurrentTime;
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &ThisClass::ReleaseNextFootprints, TimeUntilNextRelease);
	}
}

TSubclassOf<AD1FootprintActor> UD1FootprintComponent::DetermineClass(const FGameplayTagContainer& ContextTags)
{
	for (UD1FootprintStyle* Style : Styles)
	{
		if (Style && Style->bOverrideFootprint)
		{
			if (Style->MatchPattern.Matches(ContextTags) || Style->MatchPattern.IsEmpty())
			{
				return Style->FootprintClass;
			}
		}
	}
	
	return DefaultFootprintClass;
}
