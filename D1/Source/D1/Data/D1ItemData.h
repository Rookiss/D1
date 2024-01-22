#pragma once

#include "Item/Fragments/D1ItemFragment.h"
#include "D1ItemData.generated.h"

UENUM()
enum class EItemRarity : uint8
{
	Junk,
	Poor,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Unique,

	Count	UMETA(Hidden)
};

USTRUCT()
struct FD1ItemProbability
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Junk;

	UPROPERTY(EditDefaultsOnly)
	int32 Probability = 0;
};

USTRUCT()
struct FD1ItemDefinition
{
	GENERATED_BODY()

public:
	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const;
	
public:
	UPROPERTY(EditDefaultsOnly)
	int32 ItemID = 0;

	UPROPERTY(EditDefaultsOnly)
	FIntPoint ItemSlotCount = FIntPoint::ZeroValue;
	
	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> IconTexture;
	
	UPROPERTY(EditDefaultsOnly, Instanced)
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

UCLASS(Const)
class UD1ItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UD1ItemData();
	
public:
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

public:
	const TArray<FD1ItemProbability>& GetItemProbabilities() const { return ItemProbabilities; }
	const FD1ItemDefinition& GetItemDefByID(int32 ItemID) const;
	
private:
	UPROPERTY(EditDefaultsOnly, EditFixedSize, meta=(ShowInnerProperties))
	TArray<FD1ItemProbability> ItemProbabilities;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FD1ItemDefinition> ItemNameToDef;

	UPROPERTY()
	TMap<int32, FD1ItemDefinition> ItemIDToDef;
};
