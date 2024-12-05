#pragma once

#include "Components/PawnComponent.h"
#include "Feedback/ContextEffects/LyraContextEffectsInterface.h"
#include "D1FootprintComponent.generated.h"

class UD1FootprintStyle;

USTRUCT()
struct FPooledFootprintList
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> Footprints;
};

USTRUCT()
struct FLiveFootprintEntry
{
	GENERATED_BODY()

public:
	FLiveFootprintEntry() { }

	FLiveFootprintEntry(AActor* InFootprintActor, FPooledFootprintList* InPoolList, float InReleaseTime)
		: FootprintActor(InFootprintActor), PoolList(InPoolList), ReleaseTime(InReleaseTime) { }

public:
	UPROPERTY(Transient)
	TObjectPtr<AActor> FootprintActor = nullptr;

	FPooledFootprintList* PoolList = nullptr;

	float ReleaseTime = 0.f;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UD1FootprintComponent : public UPawnComponent, public ILyraContextEffectsInterface
{
	GENERATED_BODY()
	
public:
	UD1FootprintComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void AnimMotionEffect_Implementation(const FName Bone, const FGameplayTag MotionEffect, USceneComponent* StaticMeshComponent, const FVector LocationOffset, const FRotator RotationOffset, const UAnimSequenceBase* AnimationSequence, const bool bHitSuccess, const FHitResult HitResult, FGameplayTagContainer Contexts, FVector VFXScale = FVector(1), float AudioVolume = 1, float AudioPitch = 1) override;

protected:
	void ReleaseNextFootprints();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	bool bEnableFootprintEffect = true;

	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	TArray<TObjectPtr<UD1FootprintStyle>> Styles;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	TSubclassOf<AActor> DefaultFootprintLeftClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	TSubclassOf<AActor> DefaultFootprintRightClass;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	float FootprintLifeSpan = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	FName FootprintRightFootBone = "foot_r";
	
protected:
	UPROPERTY(Transient)
	TMap<TSubclassOf<AActor>, FPooledFootprintList> PooledFootprintMap;

	UPROPERTY(Transient)
	TArray<FLiveFootprintEntry> LiveFootprints;
	
	FTimerHandle ReleaseTimerHandle;
};
