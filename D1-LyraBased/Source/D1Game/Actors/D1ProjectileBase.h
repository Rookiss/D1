#pragma once

#include "GameplayTagContainer.h"
#include "AbilitySystem/D1AbilitySourceInterface.h"
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
class AD1ProjectileBase : public AActor, public ID1AbilitySourceInterface
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

public:
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	
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
	UPROPERTY(EditDefaultsOnly, Category="D1|Projectile", BlueprintReadOnly)
	float Damage = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Projectile", BlueprintReadOnly, meta=(Categories="GameplayCue"))
	FGameplayTag HitGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category="D1|Projectile", BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> HitEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Projectile", BlueprintReadOnly)
	bool bAttachToHitComponent = false;

	UPROPERTY(EditDefaultsOnly, Category="D1|Projectile", BlueprintReadOnly)
	ECollisionDetectionType CollisionDetectionType = ECollisionDetectionType::None;

	UPROPERTY(EditDefaultsOnly, Category="D1|Projectile", BlueprintReadOnly)
	FRuntimeFloatCurve DistanceDamageFalloff;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Projectile", meta=(ForceInlineRow))
	TMap<FGameplayTag, float> MaterialTagMultiplier;
	
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

	FVector OriginLocation = FVector::ZeroVector;
};
