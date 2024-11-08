#pragma once

#include "D1CheatData.generated.h"

class UGameplayEffect;

UCLASS(Const)
class UD1CheatData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1CheatData& Get();

public:
	const TArray<TSoftObjectPtr<UAnimMontage>>& GetAnimMontagePaths() const { return AnimMontagePaths; }
	const TSubclassOf<UGameplayEffect> GetResetCooldownGameplayEffectClass() const { return ResetCooldownGameplayEffectClass; }
	const TSubclassOf<UGameplayEffect> GetResetVitalGameplayEffectClass() const { return ResetVitalGameplayEffectClass; }
	
private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UAnimMontage>> AnimMontagePaths;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ResetCooldownGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ResetVitalGameplayEffectClass;
};
