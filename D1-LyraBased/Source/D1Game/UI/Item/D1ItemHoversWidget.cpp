#include "D1ItemHoversWidget.h"

#include "D1ItemHoverWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemHoversWidget)

UD1ItemHoversWidget::UD1ItemHoversWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemHoversWidget::RefreshUI(const UD1ItemInstance* HoveredItemInstance)
{
	const UD1ItemInstance* PairItemInstance = nullptr;
	if (UD1EquipmentManagerComponent* EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<UD1EquipmentManagerComponent>())
	{
		EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
		PairItemInstance = EquipmentManager->FindPairItemInstance(HoveredItemInstance, EquipmentSlotType);
	}
	
	HoverWidget_Left->RefreshUI(HoveredItemInstance);
	HoverWidget_Right->RefreshUI(PairItemInstance);
}

void UD1ItemHoversWidget::SetPosition(const FVector2D& AbsolutePosition)
{
	HorizontalBox_Hovers->ForceLayoutPrepass();
	FVector2D HoversWidgetSize = HorizontalBox_Hovers->GetDesiredSize();

	FVector2D CanvasWidgetSize = CanvasPanel_Root->GetCachedGeometry().GetLocalSize();
	if (CanvasWidgetSize.IsZero())
		return;
	
	FVector2D Margin = FVector2D(10.f, 15.f);
	FVector2D MouseWidgetPos = CanvasPanel_Root->GetCachedGeometry().AbsoluteToLocal(AbsolutePosition);
	FVector2D HoverWidgetStartPos = MouseWidgetPos + Margin;
	FVector2D HoverWidgetEndPos = HoverWidgetStartPos + HoversWidgetSize;

	FVector2D OutSize = (HoverWidgetEndPos - CanvasWidgetSize);
	if (OutSize.X > 0.f)
	{
		HoverWidgetStartPos.X -= OutSize.X;
	}
	if (OutSize.Y > 0.f)
	{
		HoverWidgetStartPos.Y -= OutSize.Y;
	}
		
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(HorizontalBox_Hovers->Slot))
	{
		CanvasPanelSlot->SetPosition(HoverWidgetStartPos);
	}
}
