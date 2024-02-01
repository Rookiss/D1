#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventorySlotWidget.generated.h"

class USizeBox;
class UImage;

UENUM()
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

protected:
	UPROPERTY(EditDefaultsOnly)
	FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 0.f);
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor RedColor = FLinearColor(0.28f, 0.02f, 0.f, 0.32f);
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor GreenColor = FLinearColor(0.02f, 0.28f, 0.f, 0.32f);
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor BlueColor = FLinearColor(0.f, 0.02f, 0.28f, 0.32f);

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
