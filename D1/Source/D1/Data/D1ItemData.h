#pragma once

#include "D1ItemData.generated.h"

class UD1ItemFragment;

USTRUCT(BlueprintType)
struct FD1ItemDefinition
{
	GENERATED_BODY()

public:
	const UD1ItemFragment* FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ID = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UD1ItemFragment>> Fragments;
};

UCLASS(BlueprintType, Const)
class UD1ItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	static const FName ItemDataName;
	
public:
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

public:
	const FD1ItemDefinition& GetItemDefByID(int32 ItemID) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FD1ItemDefinition> ItemNameToDef;

	UPROPERTY()
	TMap<int32, FD1ItemDefinition> ItemIDToDef;
};
