#pragma once

#include "GameplayTagContainer.h"
#include "D1ProjectileBase.generated.h"

class UGameplayEffect;
class UNiagaraComponent;
class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UENUM(BlueprintType)
enum class ECollisionDetectionType : uint8
{
	None,
	Hit,
	Overlap
};

UCLASS(BlueprintType, Abstract)
class AD1ProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1ProjectileBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

protected:
	UFUNCTION(BlueprintCallable)
	void SetSpeed(float Speed);
	
private:
	UFUNCTION()
	void HandleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult);

	UFUNCTION()
	void HandleComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOtherComponentDeactivated(UActorComponent* OtherComponent);

	UFUNCTION()
	void HandleCollisionDetection(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& HitResult);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="GameplayCue"))
	FGameplayTag HitGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> HitEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAttachToHitComponent = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECollisionDetectionType CollisionDetectionType = ECollisionDetectionType::None;
	
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
	UPROPERTY()
	TWeakObjectPtr<UActorComponent> AttachingComponent;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
};
