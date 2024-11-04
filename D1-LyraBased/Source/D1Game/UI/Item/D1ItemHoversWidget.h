#pragma once

#include "Blueprint/UserWidget.h"
#include "Item/D1ItemInstance.h"
#include "D1ItemHoversWidget.generated.h"

class UD1EquipmentManagerComponent;
class UD1ItemInstance;
class UCanvasPanel;
class UHorizontalBox;
class UD1ItemHoverWidget;

UCLASS()
class UD1ItemHoversWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemHoversWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(const UD1ItemInstance* HoveredItemInstance);
	void SetPosition(const FVector2D& AbsolutePosition);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Hovers;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ItemHoverWidget> HoverWidget_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ItemHoverWidget> HoverWidget_Right;
};
