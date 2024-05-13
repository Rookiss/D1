#pragma once

#include "GameplayEffectTypes.h"
#include "D1AOEBase.generated.h"

class AD1Character;
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
	UPROPERTY(EditDefaultsOnly, Category="Default")
	FGameplayTag GameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category="Default")
	int32 TargetAttackCount = 0;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float AttackIntervalTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UGameplayEffect> AdditionalGameplayEffectClass;
	
private:
	int32 CurrAttackCount = 0;
	TSet<TObjectPtr<AActor>> HitActors;
	FTimerHandle AOETimerHandle;
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
};
