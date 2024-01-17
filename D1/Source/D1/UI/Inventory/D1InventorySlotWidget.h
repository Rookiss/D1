#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventorySlotWidget.generated.h"

class UD1InventoryEntryWidget;
class USizeBox;
class UImage;

UENUM(BlueprintType)
enum class ESlotColor
{
	Normal,
	Blue,
	Green,
	Red
};

UCLASS()
class UD1InventorySlotWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

public:
	void SetEntryWidget(UD1InventoryEntryWidget* InEntryWidget) { EntryWidget = InEntryWidget; }
	UD1InventoryEntryWidget* GetEntryWidget() const { return EntryWidget; }
	
	void ChangeSlotColor(ESlotColor SlotColor);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D SlotSize = FVector2D(50, 50);

protected:
	UPROPERTY()
	TObjectPtr<UD1InventoryEntryWidget> EntryWidget;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Blue;
};
