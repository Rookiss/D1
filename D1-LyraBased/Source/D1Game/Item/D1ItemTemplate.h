#pragma once

#include "D1ItemTemplate.generated.h"

class UD1ItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UD1ItemFragment : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void OnInstanceCreated(UD1ItemInstance* Instance) const { }
};

UCLASS(Blueprintable, Const, Abstract)
class UD1ItemTemplate : public UObject
{
	GENERATED_BODY()
	
public:
	UD1ItemTemplate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType="FragmentClass"))
	const UD1ItemFragment* FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}
	
public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint SlotCount = FIntPoint::ZeroValue;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackCount = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> PickupableMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UD1ItemFragment>> Fragments;
};
