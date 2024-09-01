#pragma once

#include "D1Define.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "D1AnimNotifyState_OverlayEffect.generated.h"

USTRUCT()
struct FOverlayEffectProgressInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float ElapsedTime = 0.f;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> OverlayMaterialInstance;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<UMeshComponent>> MeshComponents;
};

UCLASS(editinlinenew, Const, hideCategories = Object, collapseCategories, MinimalAPI, Meta = (DisplayName = "Weapon Overlay Effect"))
class UD1AnimNotifyState_OverlayEffect : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UD1AnimNotifyState_OverlayEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void ApplyWeaponMeshComponent(FOverlayEffectProgressInfo& ProgressInfo, USkeletalMeshComponent* MeshComponent);
	void ApplyAllWeaponMeshComponents(FOverlayEffectProgressInfo& ProgressInfo, USkeletalMeshComponent* MeshComponent);
	void ApplyCharacterMeshComponents(FOverlayEffectProgressInfo& ProgressInfo, USkeletalMeshComponent* MeshComponent);
	
protected:
	UPROPERTY(EditAnywhere)
	EOverlayTargetType OverlayTargetType = EOverlayTargetType::None;

	UPROPERTY(EditAnywhere, meta=(EditCondition="OverlayTargetType == EOverlayTargetType::Weapon", EditConditionHides))
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveLinearColor> LinearColorCurve;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> OverlayMaterial;

	UPROPERTY(EditAnywhere)
	FName ParameterName = "Color";

	UPROPERTY(EditAnywhere)
	bool bApplyRateScaleToProgress = true;
	
protected:
	UPROPERTY()
	TMap<TWeakObjectPtr<UMeshComponent>, FOverlayEffectProgressInfo> ProgressInfoMap;
};
