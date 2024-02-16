#pragma once

#include "D1Define.h"
#include "UI/D1UserWidget.h"
#include "D1InventorySlotWidget.generated.h"

class USizeBox;
class UImage;

UCLASS()
class UD1InventorySlotWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	
public:
	void ChangeSlotState(ESlotState InSlotState);
	void ChangeHoverState(ESlotState InHoverState);

public:
	static inline FIntPoint UnitSlotSize = FIntPoint(50.f, 50.f);
	
private:
	ESlotState SlotState = ESlotState::Default;

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Foreground;
};
