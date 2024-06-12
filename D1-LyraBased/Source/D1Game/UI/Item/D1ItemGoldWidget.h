#pragma once

#include "Blueprint/UserWidget.h"
#include "Item/D1ItemInstance.h"
#include "D1ItemGoldWidget.generated.h"

class UTextBlock;

UCLASS()
class UD1ItemGoldWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemGoldWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	
private:
	void RefreshGoldCount();
	void OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 NewItemCount);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_TotalGold;

private:
	UPROPERTY()
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;
};
