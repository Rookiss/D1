#pragma once

#include "D1AOEElementBase.generated.h"

class UGameplayEffect;
class UArrowComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class UCapsuleComponent;
class UTimelineComponent;

UENUM(BlueprintType)
enum class EAOEElementType : uint8
{
	Projectile,
	Explosion,
};

UCLASS()
class AD1AOEElementBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1AOEElementBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnTimelineTick(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	bool PerformTrace(const FVector& StartLocation, const FVector& EndLocation, TArray<FHitResult>& OutHitResults);
	
	UFUNCTION()
	void HandleCollisionDetection(const FHitResult& HitResult);

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE Element")
	float Damage = 40.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|AOE Element")
	TSubclassOf<UGameplayEffect> AdditionalEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE Element")
	EAOEElementType ElementType = EAOEElementType::Projectile;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE Element", meta=(EditCondition="ElementType == EAOEElementType::Projectile", EditConditionHides))
	TObjectPtr<UCurveFloat> CurveData;

	UPROPERTY(EditDefaultsOnly, Category="D1|AOE Element", meta=(EditCondition="ElementType == EAOEElementType::Projectile", EditConditionHides))
	TObjectPtr<UNiagaraSystem> HitNiagaraEffect;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> TimelineComponent;

private:
	bool bFoundHit = false;
	FTimerHandle TempTimerHandle;
};
