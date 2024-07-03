#pragma once

#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "D1AnimNotifyState_WeaponTimedNiagaraEffect.generated.h"

UCLASS(Blueprintable, meta=(DisplayName="Weapon Timed Niagara Effect"), MinimalAPI)
class UD1AnimNotifyState_WeaponTimedNiagaraEffect : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	virtual UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const override;
};
