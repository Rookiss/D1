#pragma once

#include "D1CheatData.generated.h"

UCLASS(Const)
class UD1CheatData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1CheatData& Get();

public:
	const TArray<TSoftObjectPtr<UAnimMontage>>& GetAnimMontagePaths() const { return AnimMontagePaths; }
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UAnimMontage>> AnimMontagePaths;
};
