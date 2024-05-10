#pragma once

#include "GameplayEffectTypes.h"
#include "D1ProjectileBase.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS(BlueprintType, Abstract)
class AD1ProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1ProjectileBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	void Init(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle);
	
private:
	UFUNCTION()
	void HandleComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 5.f;
	
	UPROPERTY(EditDefaultsOnly)
	float Speed = 500.f;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> ImpactSound;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereCollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> TrailNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	
private:
	bool bHit = false;
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
};
