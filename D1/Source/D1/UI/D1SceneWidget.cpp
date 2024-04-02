#include "D1SceneWidget.h"

#include "D1SpellSelectWidget.h"
#include "D1SpellProgressWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Item/D1ItemHoverWidget.h"
#include "Item/D1PlayerInventoryWidget.h"
#include "Player/D1PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SceneWidget)

UD1SceneWidget::UD1SceneWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

void UD1SceneWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ControlledInventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	ItemHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
	SpellSelectWidget->SetVisibility(ESlateVisibility::Hidden);
	SpellProgressWidget->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UD1SceneWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	// TODO: Custom Key Binding
	if (InKeyEvent.IsRepeat() == false && InKeyEvent.GetKey() == EKeys::Tab)
	{
		FSlateApplication::Get().CancelDragDrop();
		HideControlledInventoryWidget();
		return FReply::Handled();
	}
	
	return Reply;
}

FReply UD1SceneWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	// TODO: Custom Key Binding
	if (InKeyEvent.IsRepeat() == false && InKeyEvent.GetKey() == EKeys::E)
	{
		HideSpellSelectWidget();
		return FReply::Handled();
	}

	return Reply;
}

FReply UD1SceneWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (IsAllItemSlotWidgetHidden())
	{
		HideItemHoverWidget();
		return Reply;
	}
	
	FVector2D Space = FVector2D(5.f, 5.f);
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D ItemHoverWidgetSize = ItemHoverWidget->GetCachedGeometry().Size;
	FVector2D HoverWidgetStartPos = MouseWidgetPos + Space;
	FVector2D HoverWidgetEndPos = HoverWidgetStartPos + ItemHoverWidgetSize;

	if (HoverWidgetEndPos.X > InGeometry.Size.X)
	{
		HoverWidgetStartPos.X = MouseWidgetPos.X - (Space.X + ItemHoverWidgetSize.X);
	}

	if (HoverWidgetEndPos.Y > InGeometry.Size.Y)
	{
		HoverWidgetStartPos.Y = MouseWidgetPos.Y - (Space.Y + ItemHoverWidgetSize.Y);
	}

	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(ItemHoverWidget->Slot))
	{
		CanvasPanelSlot->SetPosition(HoverWidgetStartPos);
		return FReply::Handled();
	}
	return Reply;
}

void UD1SceneWidget::ShowControlledInventoryWidget()
{
	if (ControlledInventoryWidget->GetVisibility() != ESlateVisibility::Hidden)
		return;
	
	ControlledInventoryWidget->SetVisibility(ESlateVisibility::Visible);
	
	if (AD1PlayerController* PlayerController = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		PlayerController->SetInputModeUIOnly();
	}
}

void UD1SceneWidget::HideControlledInventoryWidget()
{
	if (ControlledInventoryWidget->GetVisibility() != ESlateVisibility::Visible)
		return;
	
	ControlledInventoryWidget->SetVisibility(ESlateVisibility::Hidden);

	if (AD1PlayerController* PlayerController = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		PlayerController->SetInputModeGameOnly();
	}
}

void UD1SceneWidget::ShowItemHoverWidget(UD1ItemInstance* ItemInstance)
{
	ItemHoverWidget->RefreshUI(ItemInstance);
	ItemHoverWidget->SetVisibility(ESlateVisibility::Visible);
}

void UD1SceneWidget::HideItemHoverWidget()
{
	ItemHoverWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UD1SceneWidget::ShowSpellSelectWidget()
{
	if (SpellSelectWidget->GetVisibility() != ESlateVisibility::Hidden)
		return;

	SpellSelectWidget->ShowWidget();
	
	if (AD1PlayerController* PlayerController = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		PlayerController->SetInputModeUIOnly();
	}
}

void UD1SceneWidget::HideSpellSelectWidget()
{
	if (SpellSelectWidget->GetVisibility() != ESlateVisibility::Visible)
		return;
	
	SpellSelectWidget->HideWidget();
	
	if (AD1PlayerController* PlayerController = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		PlayerController->SetInputModeGameOnly();
	}
}

void UD1SceneWidget::ShowSpellProgressWidget(const FText& SpellName, float CastTime)
{
	SpellProgressWidget->ShowWidget(SpellName, CastTime);
}

void UD1SceneWidget::HideSpellProgressWidget()
{
	SpellProgressWidget->HideWidget();
}

bool UD1SceneWidget::IsAllItemSlotWidgetHidden() const
{
	bool bAllHidden = true;
	if (ControlledInventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		bAllHidden = false;
	}
	return bAllHidden;
}
