#include "D1AnimNotifyState_WeaponTimedNiagaraEffect.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_WeaponTimedNiagaraEffect)

void UD1AnimNotifyState_WeaponTimedNiagaraEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UD1AnimNotifyState_WeaponTimedNiagaraEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

UFXSystemComponent* UD1AnimNotifyState_WeaponTimedNiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	return Super::SpawnEffect(MeshComp, Animation);
}
