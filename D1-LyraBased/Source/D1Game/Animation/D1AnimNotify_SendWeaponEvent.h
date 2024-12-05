#pragma once

#include "D1Define.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "D1AnimNotify_SendWeaponEvent.generated.h"

class AD1EquipmentBase;

UCLASS(meta=(DisplayName="Send Weapon Event"))
class UD1AnimNotify_SendWeaponEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UD1AnimNotify_SendWeaponEvent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;
	
	UPROPERTY(EditAnywhere)
	FGameplayEventData EventData;
};
