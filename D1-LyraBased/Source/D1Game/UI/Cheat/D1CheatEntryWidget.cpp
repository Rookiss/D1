#include "D1CheatEntryWidget.h"

#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Item/D1ItemTemplate.h"
#include "Player/LyraCheatManager.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatEntryWidget)

UD1CheatEntryWidget::UD1CheatEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CheatEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Entry->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
}

void UD1CheatEntryWidget::InitializeUI(ED1CheatEntryType InCheatEntryType, TSubclassOf<UD1ItemTemplate> InItemTemplateClass, FSoftObjectPath InAnimMontage)
{
	CheatEntryType = InCheatEntryType;
	
	switch (CheatEntryType)
	{
	case ED1CheatEntryType::PrimaryWeapon:
	case ED1CheatEntryType::SecondaryWeapon:
	case ED1CheatEntryType::Armor:
		ItemTemplateClass = InItemTemplateClass;
		if (ItemTemplateClass)
		{
			if (UD1ItemTemplate* ItemTemplate = Cast<UD1ItemTemplate>(ItemTemplateClass->GetDefaultObject()))
			{
				Text_Entry->SetText(ItemTemplate->DisplayName);
				Image_Entry->SetBrushFromTexture(ItemTemplate->IconTexture, true);
			}
		}
		break;
	case ED1CheatEntryType::Animation:
		AnimMontage = InAnimMontage;
		if (AnimMontage)
		{
			Text_Entry->SetText(FText::FromString(AnimMontage.GetAssetName()));
			SizeBox_Entry->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	}
}

void UD1CheatEntryWidget::OnButtonClicked()
{
	ALyraPlayerController* LyraPlayerController = GetOwningPlayer<ALyraPlayerController>();
	if (LyraPlayerController == nullptr)
		return;
	
	if (CheatEntryType == ED1CheatEntryType::PrimaryWeapon)
	{
		LyraPlayerController->Server_EquipWeapon(EWeaponSlotType::Primary, ItemTemplateClass);
	}
	else if (CheatEntryType == ED1CheatEntryType::SecondaryWeapon)
	{
		LyraPlayerController->Server_EquipWeapon(EWeaponSlotType::Secondary, ItemTemplateClass);
	}
	else if (CheatEntryType == ED1CheatEntryType::Armor)
	{
		LyraPlayerController->Server_EquipArmor(ItemTemplateClass);
	}
	else if (CheatEntryType == ED1CheatEntryType::Animation)
	{
		if (ULyraCheatManager* LyraCheatManager = Cast<ULyraCheatManager>(LyraPlayerController->CheatManager))
		{
			LyraCheatManager->SelectedMontage = AnimMontage;
		}
	}
}
