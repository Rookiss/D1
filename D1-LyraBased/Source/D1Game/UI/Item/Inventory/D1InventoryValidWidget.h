#pragma once

#include "D1Define.h"
#include "Blueprint/UserWidget.h"
#include "D1InventoryValidWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class UD1InventoryValidWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventoryValidWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;

public:
	void ChangeSlotState(ESlotState SlotState);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;
};
