#pragma once

#include "D1Define.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "D1AnimNotifyState_PerformTrace.generated.h"

class AD1WeaponBase;

UCLASS()
class UD1AnimNotifyState_PerformTrace : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UD1AnimNotifyState_PerformTrace(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
public:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;

	UPROPERTY(EditAnywhere)
	int32 TargetFPS = 60;

	UPROPERTY(EditAnywhere)
	bool bDrawDebugShape = false;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ENetRole> ExecuteNetRole = ROLE_None;

private:
	UPROPERTY()
	TObjectPtr<AD1WeaponBase> WeaponActor;
	
	float TargetTickInterval = 0.f;
	bool bIsFirstTrace = true;
	FTransform PreviousTransform;
	float AccumulateTime = 0.f;
};
