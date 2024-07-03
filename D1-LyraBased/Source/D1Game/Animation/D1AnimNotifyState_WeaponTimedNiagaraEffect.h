#pragma once

#include "D1Define.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "D1AnimNotifyState_WeaponTimedNiagaraEffect.generated.h"

class AD1WeaponBase;
class UMeshComponent;
class UNiagaraSystem;
class USkeletalMeshComponent;
class UFXSystemAsset;
class UFXSystemComponent;
class UAnimInstance;

UCLASS(Blueprintable, meta=(DisplayName="Weapon Timed Niagara Effect"), MinimalAPI)
class UD1AnimNotifyState_WeaponTimedNiagaraEffect : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UD1AnimNotifyState_WeaponTimedNiagaraEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComponent, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComponent, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) const;
	UFXSystemComponent* GetSpawnedEffect(UMeshComponent* MeshComponent);
	FName GetSpawnedComponentTag()const { return GetFName(); }
	FString GetNotifyName_Implementation() const override;
	bool ValidateParameters(USkeletalMeshComponent* MeshComp) const;
	
protected:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (DisplayName = "Niagara System", ToolTip = "The niagara system to spawn for the notify state"))
	TObjectPtr<UNiagaraSystem> Template;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "The socket or bone to attach the system to", AnimNotifyBoneName = "true"))
	FName SocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "Offset from the socket or bone to place the Niagara system"))
	FVector LocationOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "Rotation offset from the socket or bone for the Niagara system"))
	FRotator RotationOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem)
	bool bApplyRateScaleAsTimeDilation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (DisplayName = "Destroy Immediately", ToolTip = "Whether the Niagara system should be immediately destroyed at the end of the notify state or be allowed to finish"))
	bool bDestroyAtEnd = false;
	
private:
	UPROPERTY()
	TWeakObjectPtr<AD1WeaponBase> CachedWeaponActor;
};
