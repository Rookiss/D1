#pragma once

#include "D1Define.h"
#include "AnimNotify_PlayNiagaraEffect.h"
#include "D1AnimNotify_WeaponNiagaraEffect.generated.h"

UCLASS(meta=(DisplayName="Weapon Niagara Effect"))
class UD1AnimNotify_WeaponNiagaraEffect : public UAnimNotify_PlayNiagaraEffect
{
	GENERATED_BODY()
	
public:
	UD1AnimNotify_WeaponNiagaraEffect();

public:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
private:
	USkeletalMeshComponent* GetWeaponMeshComponent(USkeletalMeshComponent* MeshComponent) const;

protected:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;
};
