#pragma once

#include "UI/Item/D1ItemSlotWidget.h"
#include "D1InventorySlotWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class UD1InventorySlotWidget : public UD1ItemSlotWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot;
};
