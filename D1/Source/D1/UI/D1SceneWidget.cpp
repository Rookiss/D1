#include "D1SceneWidget.h"

#include "Character/D1Player.h"
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

	ControlledPlayerInventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	ItemHoverWidget->SetVisibility(ESlateVisibility::Hidden);
	
	if (AD1Player* Player = Cast<AD1Player>(GetOwningPlayerPawn()))
	{
		ControlledPlayerInventoryWidget->Init(Player->EquipmentManagerComponent, Player->InventoryManagerComponent);
	}
}

FReply UD1SceneWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	
	if (InKeyEvent.IsRepeat() == false && InKeyEvent.GetKey() == EKeys::I)
	{
		HideControlledPlayerInventoryWidget();
		return FReply::Handled();
	}
	
	return Reply;
}

FReply UD1SceneWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	if (IsAllHidden())
		return Reply;
	
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D Space = FVector2D(5.f, 5.f);
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

void UD1SceneWidget::ShowControlledPlayerInventoryWidget()
{
	if (ControlledPlayerInventoryWidget->GetVisibility() != ESlateVisibility::Hidden)
		return;
	
	ControlledPlayerInventoryWidget->SetVisibility(ESlateVisibility::Visible);
	
	if (AD1PlayerController* PlayerController = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		PlayerController->SetInputModeUIOnly(this);
	}
}

void UD1SceneWidget::HideControlledPlayerInventoryWidget()
{
	if (ControlledPlayerInventoryWidget->GetVisibility() != ESlateVisibility::Visible)
		return;
	
	ControlledPlayerInventoryWidget->SetVisibility(ESlateVisibility::Hidden);

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

bool UD1SceneWidget::IsAllHidden() const
{
	bool bAllHidden = true;
	if (ControlledPlayerInventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		bAllHidden = false;
	}
	return bAllHidden;
}
