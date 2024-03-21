#pragma once

#include "D1Define.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "D1AnimNotifyState_PerformTrace.generated.h"

UENUM(BlueprintType)
enum class ETraceType : uint8
{
	Distance,
	Frame
};

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
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void PerformTrace(USkeletalMeshComponent* MeshComponent, float DeltaTime);
	
public:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;

	UPROPERTY(EditAnywhere)
	FName TraceSocketName = "TraceSocket";
	
	UPROPERTY(EditAnywhere)
	ETraceType TraceType = ETraceType::Distance;

	UPROPERTY(EditAnywhere)
	float TargetDistance = 20.f;
	
	UPROPERTY(EditAnywhere)
	int32 TargetFPS = 60;

	UPROPERTY(EditAnywhere)
	bool bDrawDebugShape = false;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ENetRole> ExecuteNetRole = ROLE_Authority;

private:
	UPROPERTY()
	TObjectPtr<AD1WeaponBase> WeaponActor;

	float Distance = 0.f;
	double LastTickTime = 0;
	float TargetDeltaTime = 0.f;
	FTransform PreviousTraceTransform;
	FTransform PreviousDebugTransform;
	FTransform PreviousSocketTransform;
};
