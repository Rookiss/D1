#pragma once

#include "GameplayEffectTypes.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void InitializedWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	// TODO: For Test
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Pitch = 0.f;
};
