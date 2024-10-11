#pragma once

#include "D1Define.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "D1AnimNotifyState_WeaponTrailNiagaraEffect.generated.h"

UCLASS(Blueprintable, meta=(DisplayName="Weapon Trail Niagara Effect"), MinimalAPI)
class UD1AnimNotifyState_WeaponTrailNiagaraEffect : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()
	
public:
	UD1AnimNotifyState_WeaponTrailNiagaraEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComponent, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComponent, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void UpdateNiagaraParameters(USkeletalMeshComponent* WeaponMeshComponent);
	USkeletalMeshComponent* GetWeaponMeshComponent(USkeletalMeshComponent* CharacterMeshComponent) const;
	
protected:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "The socket or bone to attach the system to", AnimNotifyBoneName = "true"))
	FName StartSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem)
	FName StartParameterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "The socket or bone to attach the system to", AnimNotifyBoneName = "true"))
	FName EndSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem)
	FName EndParameterName;
};
