#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "D1InventoryStatWidget.generated.h"

class UD1SkillStatHoverWidget;

UCLASS()
class UD1InventoryStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventoryStatWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeDestruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag StatTag;
	
protected:
	UPROPERTY()
	TObjectPtr<UD1SkillStatHoverWidget> HoverWidget;
};
