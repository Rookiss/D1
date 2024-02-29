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
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Direction = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	float AimPitch = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouching = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly)
	float LeftHandWeight = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float RightHandWeight = 0.f;
};
