#pragma once

#include "D1Define.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "D1AnimNotifyState_OverlayEffect.generated.h"

UENUM(BlueprintType)
enum class EOverlayTargetType : uint8
{
	None,
	Weapon,
	Character,
	All,
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
	void ApplyWeaponMeshComponent(USkeletalMeshComponent* MeshComponent);
	void ApplyAllWeaponMeshComponents(USkeletalMeshComponent* MeshComponent);
	void ApplyCharacterMeshComponents(USkeletalMeshComponent* MeshComponent);
	void Clear();
	
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
	
private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> OverlayMaterialInstance;

	UPROPERTY()
	TArray<TWeakObjectPtr<UMeshComponent>> CachedMeshComponents;

	float PassedTime = 0.f;
};
