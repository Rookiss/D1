#include "D1FootprintComponent.h"

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

	ALyraCharacter* LyraCharacter = GetPawn<ALyraCharacter>();
	if (LyraCharacter == nullptr)
		return;

	USkeletalMeshComponent* CharacterMeshComponent = LyraCharacter->GetMesh();
	
	TSubclassOf<AActor> FootprintClassToUse = (Bone == FootprintRightFootBone) ? DefaultFootprintRightClass : DefaultFootprintLeftClass;
	if (FootprintClassToUse == nullptr)
		return;

	FPooledFootprintList& FootprintPool = PooledFootprintMap.FindOrAdd(FootprintClassToUse);
	
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
	bool bFootprintSide = (Bone == FootprintRightFootBone);
	
	FName CurrentSocketName = FName(*UKismetStringLibrary::Concat_StrStr(Bone.ToString(), TEXT("_Socket")));
	FTransform FootprintSocketTransform = CharacterMeshComponent->GetSocketTransform(CurrentSocketName, RTS_World);

	FVector SocketForward = UKismetMathLibrary::TransformDirection(FootprintSocketTransform, FVector(1.f, 0.f, 0.f));

	FVector FloorUp = HitResult.Normal;
	FVector FloorRight = FVector::CrossProduct(FloorUp, SocketForward).GetSafeNormal();
	FVector FloorForward = FVector::CrossProduct(FloorRight, FloorUp);

	FQuat FootprintOrientation = UKismetMathLibrary::MakeRotationFromAxes(FloorUp, FloorRight, FloorForward).Quaternion();
}

void UD1FootprintComponent::ReleaseNextFootprints()
{
	
}
