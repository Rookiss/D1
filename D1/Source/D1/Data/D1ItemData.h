#pragma once

#include "D1Define.h"
#include "Item/Fragments/D1ItemFragment.h"
#include "D1ItemData.generated.h"

USTRUCT()
struct FD1ItemRarityProbability
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Junk;

	UPROPERTY(EditDefaultsOnly)
	int32 Probability = 0;
};

USTRUCT()
struct FD1ItemTemplate
{
	GENERATED_BODY()

public:
	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const;
	
public:
	UPROPERTY(EditDefaultsOnly)
	int32 TemplateID = 0;

	UPROPERTY(EditDefaultsOnly)
	FIntPoint SlotCount = FIntPoint::ZeroValue;
	
	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> IconTexture;
	
	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<TObjectPtr<UD1ItemFragment>> Fragments;
};

template <typename FragmentClass>
const FragmentClass* FD1ItemTemplate::FindFragmentByClass() const
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

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
public:
	const TArray<FD1ItemRarityProbability>& GetRarityProbabilities() const { return RarityProbabilities; }
	const FD1ItemTemplate& FindItemTemplateByID(int32 TemplateID) const;

private:
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FD1ItemRarityProbability> RarityProbabilities;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FD1ItemTemplate> NameToTemplate;

	UPROPERTY()
	TMap<int32, FD1ItemTemplate> IDToTemplate;
};
