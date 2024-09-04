#pragma once

#include "D1Define.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "D1AnimNotifyState_PlayWeaponSound.generated.h"

UENUM(BlueprintType)
enum class EWeaponSoundType : uint8
{
	None,
	Swing,
	Custom,
};

UCLASS(meta=(DisplayName="Play Weapon Sound"))
class UD1AnimNotifyState_PlayWeaponSound : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UD1AnimNotifyState_PlayWeaponSound(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;

	UPROPERTY(EditAnywhere)
	EWeaponSoundType WeaponSoundType = EWeaponSoundType::None;

	UPROPERTY(EditAnywhere, meta=(EditCondition="WeaponSoundType == EWeaponSoundType::Custom"))
	TObjectPtr<USoundBase> CustomSound;
	
private:
	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent;
};
