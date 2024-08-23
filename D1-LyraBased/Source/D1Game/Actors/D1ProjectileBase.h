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
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	UFUNCTION(BlueprintCallable)
	void Init(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle);

private:
	UFUNCTION()
	void HandleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult);

	UFUNCTION()
	void HandleOtherComponentDeactivated(UActorComponent* OtherComponent);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ProjectileMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> TrailNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

private:
	bool bHit = false;
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY()
	TWeakObjectPtr<UActorComponent> OtherHitComponent;
};
