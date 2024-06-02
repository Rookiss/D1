#pragma once

#include "Item/D1ItemDefinition.h"
#include "D1ItemData.generated.h"

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

UCLASS(BlueprintType, Const, meta=(DisplayName="D1 Item Data"))
class UD1ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UD1ItemData& Get();
	
public:
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
public:
	const FD1ItemTemplate& FindItemTemplateByID(int32 TemplateID) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FD1ItemTemplate> ItemNameToTemplate;

	UPROPERTY()
	TMap<int32, FD1ItemTemplate> ItemIDToTemplate;
};
