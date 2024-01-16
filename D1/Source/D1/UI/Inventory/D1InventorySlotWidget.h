#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventorySlotWidget.generated.h"

class UImage;

UCLASS()
class UD1InventorySlotWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Selected;
};
