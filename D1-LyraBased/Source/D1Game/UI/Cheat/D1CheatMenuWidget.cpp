#include "D1CheatMenuWidget.h"

#include "D1CheatEntryWidget.h"
#include "D1CheatListWidget.h"
#include "Components/Button.h"
#include "Data/D1CheatData.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Fragments/D1ItemFragment_Equipable.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatMenuWidget)

UD1CheatMenuWidget::UD1CheatMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CheatMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CheatEntryWidgetClass = ULyraAssetManager::GetSubclassByName<UD1CheatEntryWidget>("CheatEntryWidgetClass");
	Button_Exit->OnClicked.AddUniqueDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UD1CheatMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const UD1ItemData& ItemData = ULyraAssetManager::Get().GetItemData();

	TArray<TSubclassOf<UD1ItemTemplate>> ItemTemplateClasses;
	ItemData.GetAllItemTemplateClasses(ItemTemplateClasses);

	for (TSubclassOf<UD1ItemTemplate> ItemTemplateClass : ItemTemplateClasses)
	{
		const UD1ItemTemplate* ItemTemplate = ItemTemplateClass.GetDefaultObject();
		if (const UD1ItemFragment_Equipable* EquippableFragment = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Equipable>())
		{
			if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
			{
				const UD1ItemFragment_Equipable_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Equipable_Weapon>();
				if (WeaponFragment->WeaponType == EWeaponType::Unarmed)
					continue;
				
				UD1CheatEntryWidget* CheatEntryWidget = CreateWidget<UD1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ED1CheatEntryType::PrimaryWeapon, ItemTemplateClass, nullptr);
				CheatList_PrimaryWeapon->AddEntry(CheatEntryWidget);

				CheatEntryWidget = CreateWidget<UD1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ED1CheatEntryType::SecondaryWeapon, ItemTemplateClass, nullptr);
				CheatList_SecondaryWeapon->AddEntry(CheatEntryWidget);
			}
			else if (EquippableFragment->EquipmentType == EEquipmentType::Utility)
			{
				UD1CheatEntryWidget* CheatEntryWidget = CreateWidget<UD1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ED1CheatEntryType::Utility, ItemTemplateClass, nullptr);
				CheatList_Utility->AddEntry(CheatEntryWidget);
			}
			else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
			{
				UD1CheatEntryWidget* CheatEntryWidget = CreateWidget<UD1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ED1CheatEntryType::Armor, ItemTemplateClass, nullptr);
				CheatList_Armor->AddEntry(CheatEntryWidget);
			}
		}
	}

	const UD1CheatData& CheatData = ULyraAssetManager::Get().GetCheatData();
	
	for (const TSoftObjectPtr<UAnimMontage>& AnimMontagePath : CheatData.GetAnimMontagePaths())
	{
		UD1CheatEntryWidget* CheatEntryWidget = CreateWidget<UD1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
		CheatEntryWidget->InitializeUI(ED1CheatEntryType::Animation, nullptr, AnimMontagePath);
		CheatList_Animation->AddEntry(CheatEntryWidget);
	}
}

void UD1CheatMenuWidget::OnExitButtonClicked()
{
	DeactivateWidget();
}
