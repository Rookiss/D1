#pragma once

#include "InventoryItemFragment.h"
#include "InventoryItemFragment_DisplayInfo.generated.h"

UCLASS(BlueprintType)
class UInventoryItemFragment_DisplayInfo : public UInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UInventoryItemFragment_DisplayInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSlateBrush IconBrush;
};
