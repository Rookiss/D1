#include "D1ItemEntryWidget.h"

#include "D1GameplayTags.h"
#include "Components/Image.h"
#include "Data/D1ItemData.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "System/LyraAssetManager.h"
#include "D1ItemDragWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemEntryWidget)

UD1ItemEntryWidget::UD1ItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Image_Hover->SetRenderOpacity(0.f);
	DragWidgetClass = ULyraAssetManager::GetSubclassByName<UD1ItemDragWidget>("DragWidgetClass");
}

void UD1ItemEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Image_Hover->SetRenderOpacity(1.f);

	FItemHoverWidgetVisibilityMessage Message;
	Message.bVisible = true;
	Message.ItemInstance = ItemInstance;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Item_HoverWidgetVisibility, Message);

	// TODO:
	// if (AD1HUD* HUD = Cast<AD1HUD>(GetOwningPlayer()->GetHUD()))
	// {
	// 	HUD->ShowItemHoverWidget(ItemInstance);
	// }
}

void UD1ItemEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetRenderOpacity(0.f);

	FItemHoverWidgetVisibilityMessage Message;
	Message.bVisible = false;
	Message.ItemInstance = nullptr;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Item_HoverWidgetVisibility, Message);

	// TODO:
	// if (AD1HUD* HUD = Cast<AD1HUD>(GetOwningPlayer()->GetHUD()))
	// {
	// 	HUD->HideItemHoverWidget();
	// }
}

FReply UD1ItemEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply;
}

void UD1ItemEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	RefreshWidgetOpacity(false);
}

void UD1ItemEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	RefreshWidgetOpacity(true);
}

void UD1ItemEntryWidget::RefreshWidgetOpacity(bool bClearlyVisible)
{
	SetRenderOpacity(bClearlyVisible ? 1.f : 0.5f);
}

void UD1ItemEntryWidget::SetItemInstance(UD1ItemInstance* NewItemInstance)
{
	ItemInstance = NewItemInstance;
	check(ItemInstance);

	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
}
