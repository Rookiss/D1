#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventorySlotWidget.generated.h"

class UD1InventoryEntryWidget;
class USizeBox;
class UImage;

UENUM(BlueprintType)
enum class ESlotState : uint8
{
	Default,
	InValid,
	Valid
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
	void ChangeSlotState(ESlotState InSlotState);
	void ChangeHoverState(ESlotState InHoverState);

public:
	void SetEntryWidget(UD1InventoryEntryWidget* InEntryWidget) { EntryWidget = InEntryWidget; }
	UD1InventoryEntryWidget* GetEntryWidget() const { return EntryWidget; }

	FVector2D GetSlotSize() const { return SlotSize; }

protected:
	UPROPERTY(EditDefaultsOnly)
	FVector2D SlotSize = FVector2D(50, 50);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 0.f);
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor RedColor = FLinearColor(0.28f, 0.02f, 0.f, 0.32f);
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor GreenColor = FLinearColor(0.02f, 0.28f, 0.f, 0.32f);
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor BlueColor = FLinearColor(0.f, 0.02f, 0.28f, 0.32f);

	UPROPERTY(VisibleAnywhere)
	ESlotState SlotState = ESlotState::Default;
	
protected:
	UPROPERTY()
	TObjectPtr<UD1InventoryEntryWidget> EntryWidget;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Foreground;
};
