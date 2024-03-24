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

USTRUCT(BlueprintType)
struct FTraceParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ETraceType TraceType = ETraceType::Distance;

	UPROPERTY(EditAnywhere, meta=(EditCondition="TraceType==ETraceType::Distance", EditConditionHides))
	float TargetDistance = 20.f;

	UPROPERTY(EditAnywhere, meta=(EditCondition="TraceType==ETraceType::Distance", EditConditionHides))
	FName TraceSocketName = "TraceSocket";
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="TraceType==ETraceType::Frame", EditConditionHides))
	int32 TargetFPS = 60;
};

USTRUCT(BlueprintType)
struct FTraceDebugParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool bDrawDebugShape = false;
	
	UPROPERTY(EditAnywhere)
	FColor TraceColor = FColor::Red;

	UPROPERTY(EditAnywhere)
	FColor HitColor = FColor::Green;
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
	TEnumAsByte<ENetRole> ExecuteNetRole = ROLE_Authority;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
	
	UPROPERTY(EditAnywhere)
	FTraceParams TraceParams;

	UPROPERTY(EditAnywhere)
	FTraceDebugParams TraceDebugParams;

private:
	UPROPERTY()
	TObjectPtr<AD1WeaponBase> WeaponActor;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
	
	double LastTickTime = 0;
	float TargetDeltaTime = 0.f;
	FTransform PreviousTraceTransform;
	FTransform PreviousDebugTransform;
	FTransform PreviousSocketTransform;
};
