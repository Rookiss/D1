#pragma once
#include "LyraGameplayAbility.h"

#include "D1GameplayAbility_Knockback.generated.h"

UCLASS()
class UD1GameplayAbility_Knockback : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Knockback(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	UAnimMontage* SelectDirectionalMontage(AActor* Source, AActor* Target) const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> ForwardMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LeftMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> RightMontage;
};
