#pragma once

#include "GameplayEffectTypes.h"
#include "D1AOEBase.generated.h"

class UGameplayEffect;
class UBoxComponent;
class UArrowComponent;

UCLASS(BlueprintType, Abstract)
class AD1AOEBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1AOEBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	void Init(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle);

private:
	void BeginAOE();
	void TickAOE();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> HitBoxComponent;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="AOE")
	FGameplayTag GameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category="AOE")
	int32 TargetAttackCount = 0;

	UPROPERTY(EditDefaultsOnly, Category="AOE")
	float AttackIntervalTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="AOE")
	TSubclassOf<UGameplayEffect> FirstHitGameplayEffectClass;
	
private:
	int32 CurrAttackCount = 0;
	TSet<TObjectPtr<AActor>> HitActors;
	FTimerHandle AOETimerHandle;
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
};
