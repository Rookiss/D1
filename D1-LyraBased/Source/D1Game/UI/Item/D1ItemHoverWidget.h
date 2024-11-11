#pragma once

#include "Item/D1ItemInstance.h"
#include "UI/D1HoverWidget.h"
#include "D1ItemHoverWidget.generated.h"

class UD1EquipmentManagerComponent;
class UD1ItemInstance;
class UCanvasPanel;
class UHorizontalBox;
class UD1ItemHoverEntryWidget;

UCLASS()
class UD1ItemHoverWidget : public UD1HoverWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(const UD1ItemInstance* HoveredItemInstance);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ItemHoverEntryWidget> HoverWidget_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ItemHoverEntryWidget> HoverWidget_Right;
};
