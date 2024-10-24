#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "D1GameplayAbility_Dash.generated.h"

class UAnimMontage;

UENUM()
enum class EDashDirection
{
	Left,
	Right,
	Upward,
	Downward
};

UCLASS()
class UD1GameplayAbility_Dash : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Dash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	UAnimMontage* SelectDirectionalMontage(ED1Direction MovementDirection) const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LeftMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> RightMontage;
};
