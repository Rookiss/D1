#pragma once

#include "D1AssetData.generated.h"

USTRUCT(BlueprintType)
struct FAssetEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AssetName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSoftObjectPath AssetPath;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> AssetLabels;
};

USTRUCT(BlueprintType)
struct FAssetSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAssetEntry> AssetEntries;
};

UCLASS(BlueprintType, Const, CollapseCategories)
class UD1AssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const FName AssetDataName;

public:
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
	
public:
	FSoftObjectPath GetAssetPathByName(const FName& AssetName);
	const FAssetSet& GetAssetSetByLabel(const FName& Label);
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FAssetSet> AssetGroupNameToSet;
	
	UPROPERTY()
	TMap<FName, FSoftObjectPath> AssetNameToPath;

	UPROPERTY()
	TMap<FName, FAssetSet> AssetLabelToSet;
};
