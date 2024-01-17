#pragma once
#include "Inventory/Fragments/D1ItemFragment.h"

#include "D1ItemData.generated.h"

class UD1ItemFragment;

USTRUCT(BlueprintType)
struct FD1ItemDefinition
{
	GENERATED_BODY()

public:
	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemID = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIntPoint ItemSlotCount = FIntPoint::ZeroValue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UD1ItemFragment>> Fragments;
};

template <typename FragmentClass>
const FragmentClass* FD1ItemDefinition::FindFragmentByClass() const
{
	for (UD1ItemFragment* Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass::StaticClass()))
		{
			return Cast<FragmentClass>(Fragment);
		}
	}
	return nullptr;
}

UCLASS(BlueprintType, Const)
class UD1ItemData : public UDataAsset
{
	GENERATED_BODY()
	
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
