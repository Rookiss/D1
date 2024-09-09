#pragma once

#include "D1ClassData.generated.h"

class ULyraAbilitySet;
class UTexture2D;
class ULyraGameplayAbility;

USTRUCT()
struct FClassEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText ClassName;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> ClassIcon;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULyraAbilitySet> ClassAbilitySet;
};

UCLASS(BlueprintType, Const)
class UD1ClassData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1ClassData& Get();

public:
	const TArray<FClassEntry>& GetClassEntries() const { return ClassEntries; }
	
private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FClassEntry> ClassEntries;
};
