#pragma once

#include "GameplayEffectTypes.h"
#include "D1AnimInstance.generated.h"

class UAbilitySystemComponent;

UCLASS()
class UD1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UD1AnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeInitializeAnimation() override;
	virtual void InitializedWithAbilitySystem(UAbilitySystemComponent* ASC);
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Direction = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AimPitch = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bShouldMove = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsCrouching = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFalling = false;
};
