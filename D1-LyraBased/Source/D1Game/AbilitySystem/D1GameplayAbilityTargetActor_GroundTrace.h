#pragma once

#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "D1GameplayAbilityTargetActor_GroundTrace.generated.h"

// TODO: 엔진 소스 빌드를 사용하여 AGameplayAbilityTargetActor_GroundTrace를 상속 받아서 오버라이딩 할 수 있으면 좋을듯
// TODO: 팀원들의 컴퓨터에서 직접 빌드를 하는게 아니라 엔진과 게임을 내 컴퓨터에서만 빌드하고 DLL 파일을 공유하는 방식으로 하면 되지 않을까?

UCLASS(Blueprintable)
class AD1GameplayAbilityTargetActor_GroundTrace : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()

public:
	AD1GameplayAbilityTargetActor_GroundTrace(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void StartTargeting(UGameplayAbility* InAbility) override;
	virtual bool AdjustCollisionResultForShape(const FVector OriginalStartPoint, const FVector OriginalEndPoint, const FCollisionQueryParams Params, FHitResult& OutHitResult) const;
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
	virtual bool IsConfirmTargetingAllowed() override;

protected:
	/** Radius for a sphere or capsule. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Targeting)
	float CollisionRadius;

	/** Height for a capsule. Implicitly indicates a capsule is desired if this is greater than zero. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Targeting)
	float CollisionHeight;
	
	FCollisionShape CollisionShape;
	float CollisionHeightOffset;		//When tracing, give this much extra height to avoid start-in-ground problems. Dealing with thick placement actors while standing near walls may be trickier.
	bool bLastTraceWasGood;
};
