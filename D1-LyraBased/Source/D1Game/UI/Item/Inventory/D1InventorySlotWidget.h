#pragma once

#include "Blueprint/UserWidget.h"
#include "D1InventorySlotWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class UD1InventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
};
