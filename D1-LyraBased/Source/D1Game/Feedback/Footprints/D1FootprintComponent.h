#pragma once

#include "Components/PawnComponent.h"
#include "Feedback/ContextEffects/LyraContextEffectsInterface.h"
#include "D1FootprintComponent.generated.h"

class AD1FootprintActor;
class UD1FootprintStyle;

USTRUCT()
struct FPooledFootprintList
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AD1FootprintActor>> Footprints;
};

USTRUCT()
struct FLiveFootprintEntry
{
	GENERATED_BODY()

public:
	FLiveFootprintEntry() { }

	FLiveFootprintEntry(AD1FootprintActor* InFootprintActor, FPooledFootprintList* InPoolList, float InReleaseTime)
		: FootprintActor(InFootprintActor), PoolList(InPoolList), ReleaseTime(InReleaseTime) { }

public:
	UPROPERTY(Transient)
	TObjectPtr<AD1FootprintActor> FootprintActor = nullptr;

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void ReleaseNextFootprints();

private:
	TSubclassOf<AD1FootprintActor> DetermineClass(const FGameplayTagContainer& ContextTags);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	bool bEnableFootprintEffect = true;

	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	TArray<TObjectPtr<UD1FootprintStyle>> Styles;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	TSubclassOf<AD1FootprintActor> DefaultFootprintClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	float FootprintLifeSpan = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Footprint Pool")
	FName FootprintRightFootBone = "foot_r";
	
protected:
	UPROPERTY(Transient)
	TMap<TSubclassOf<AD1FootprintActor>, FPooledFootprintList> PooledFootprintMap;

	UPROPERTY(Transient)
	TArray<FLiveFootprintEntry> LiveFootprints;
	
	FTimerHandle ReleaseTimerHandle;
};
