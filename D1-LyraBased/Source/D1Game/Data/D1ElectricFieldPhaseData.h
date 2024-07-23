#pragma once

#include "D1ElectricFieldPhaseData.generated.h"

USTRUCT(BlueprintType)
struct FD1ElectricFieldPhaseEntry
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BreakTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoticeTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShrinkTime = 0.f;
};

UCLASS(BlueprintType, Const)
class UD1ElectricFieldPhaseData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UD1ElectricFieldPhaseData& Get();

public:
	UFUNCTION(BlueprintCallable)
	const FD1ElectricFieldPhaseEntry& GetPhaseEntry(int32 PhaseIndex) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FD1ElectricFieldPhaseEntry> PhaseEntries;
};
