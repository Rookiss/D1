#pragma once

#include "D1Define.h"
#include "AnimNotify_GameplayCue.h"
#include "D1AnimNotifyState_WeaponGameplayCue.generated.h"

UCLASS(editinlinenew, Const, hideCategories = Object, collapseCategories, MinimalAPI, Meta = (DisplayName = "Weapon GameplayCue (Looping)"))
class UD1AnimNotifyState_WeaponGameplayCue : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UD1AnimNotifyState_WeaponGameplayCue();

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	FString GetNotifyName_Implementation() const override;

#if WITH_EDITOR
	virtual bool CanBePlaced(UAnimSequenceBase* Animation) const override;
#endif // #if WITH_EDITOR
	
private:
	USkeletalMeshComponent* GetWeaponMeshComponent(USkeletalMeshComponent* CharacterMeshComponent) const;
	
protected:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;

	// GameplayCue tag to invoke
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayCue, meta = (Categories = "GameplayCue"))
	FGameplayCueTag GameplayCue;

#if WITH_EDITORONLY_DATA
	FGameplayCueProxyTick PreviewProxyTick;
#endif // #if WITH_EDITORONLY_DATA
};
