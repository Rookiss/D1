#pragma once

#include "LyraGameplayAbility.h"
#include "D1GameplayAbility_Dash.generated.h"

class UAnimMontage;

UCLASS()
class UD1GameplayAbility_Dash : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Dash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void GetDirection(FVector& OutFacing, FVector& OutLastMovementInput, FVector& OutMovement) const;

	UFUNCTION(BlueprintCallable)
	UAnimMontage* SelectDirectionalMontage(const FVector& FacingDirection, const FVector& MovementDirection, bool& bOutForwardMovement, bool& bOutLeftMovement) const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LeftMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> RightMontage;
};
