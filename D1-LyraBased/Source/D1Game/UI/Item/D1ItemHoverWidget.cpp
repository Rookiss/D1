#include "D1ItemHoverWidget.h"

#include "D1ItemEntryWidget.h"
#include "GameplayEffect.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Utility.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Kismet/KismetStringLibrary.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemHoverWidget)

UD1ItemHoverWidget::UD1ItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemHoverWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (bIsEquippedWidget)
	{
		Text_IsEquipped->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Text_IsEquipped->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UD1ItemHoverWidget::RefreshUI(const UD1ItemInstance* ItemInstance)
{
	// TODO: Support Multi-Langauge
	
	if (ItemInstance == nullptr)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	const EItemRarity ItemRarity = ItemInstance->GetItemRarity();
	
	Text_DisplayName->SetText(ItemTemplate.DisplayName);
	Text_DisplayName->SetColorAndOpacity(Item::ItemRarityTextColors[(int32)ItemRarity]);
	Image_DIsplayName_Background->SetColorAndOpacity(Item::ItemRarityBackgroundColors[(int32)ItemRarity]);
	
	Text_AttributeModifiers->SetVisibility(ESlateVisibility::Collapsed);
	Text_Description->SetVisibility(ESlateVisibility::Collapsed);
	
	HorizontalBox_WeaponType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_WeaponHandType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_ArmorType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_MaxStackCount->SetVisibility(ESlateVisibility::Collapsed);

	FString ItemRarityString;
	switch (ItemRarity)
	{
	case EItemRarity::Junk:			ItemRarityString = TEXT("Junk");		break;
	case EItemRarity::Poor:			ItemRarityString = TEXT("Poor");		break;
	case EItemRarity::Normal:		ItemRarityString = TEXT("Normal");		break;
	case EItemRarity::Special:		ItemRarityString = TEXT("Special");		break;
	case EItemRarity::Rare:			ItemRarityString = TEXT("Rare");		break;
	case EItemRarity::Epic:			ItemRarityString = TEXT("Epic");		break;
	case EItemRarity::Legendary:	ItemRarityString = TEXT("Legendary");	break;
	}
	Text_ItemRarity->SetText(FText::FromString(ItemRarityString));
	Text_ItemRarity->SetColorAndOpacity(Item::ItemRarityTextColors[(int32)ItemRarity]);
	
	if (const UD1ItemFragment_Equippable* EquippableFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable>())
	{
		FString AttributeString;
		
		const FD1GameplayTagStackContainer& StackContainer = ItemInstance->GetStatContainer();
		for (const FD1GameplayTagStack& Stack : StackContainer.GetStacks())
		{
			FString Left, Right;
			Stack.GetStackTag().ToString().Split(TEXT("."), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			AttributeString.Append(FString::Printf(TEXT("%s %d\n"), *Right, Stack.GetStackCount()));
		}
		AttributeString.RemoveFromEnd(TEXT("\n"));
		
		if (AttributeString.IsEmpty() == false)
		{
			Text_AttributeModifiers->SetText(FText::FromString(AttributeString));
			Text_AttributeModifiers->SetVisibility(ESlateVisibility::Visible);
		}
		
		if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			const UD1ItemFragment_Equippable_Armor* ArmorFragment = Cast<UD1ItemFragment_Equippable_Armor>(EquippableFragment);
			Text_ItemType->SetText(FText::FromString(TEXT("Armor")));
	
			FString ArmorTypeString;
			switch (ArmorFragment->ArmorType)
			{
			case EArmorType::Helmet:	        ArmorTypeString = TEXT("Helmet");	        break;
			case EArmorType::Chest:		        ArmorTypeString = TEXT("Chest");	        break;
			case EArmorType::Legs:		        ArmorTypeString = TEXT("Legs");				break;
			case EArmorType::Hands:		        ArmorTypeString = TEXT("Hands");	        break;
			case EArmorType::Foot:		        ArmorTypeString = TEXT("Foot");				break;
			}
			Text_ArmorType->SetText(FText::FromString(ArmorTypeString));
			HorizontalBox_ArmorType->SetVisibility(ESlateVisibility::Visible);
		}
		else if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			const UD1ItemFragment_Equippable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(EquippableFragment);
			Text_ItemType->SetText(FText::FromString(TEXT("Weapon")));
			
			FString WeaponTypeString;
			switch (WeaponFragment->WeaponType)
			{
			case EWeaponType::Sword:		    WeaponTypeString = TEXT("Sword");			break;
			case EWeaponType::Shield:		    WeaponTypeString = TEXT("Shield");			break;
			case EWeaponType::Bow:				WeaponTypeString = TEXT("Bow");				break;
			case EWeaponType::Staff:			WeaponTypeString = TEXT("Staff");			break;
			}
			Text_WeaponType->SetText(FText::FromString(WeaponTypeString));
			HorizontalBox_WeaponType->SetVisibility(ESlateVisibility::Visible);
	
			FString EquipHandTypeString;
			switch (WeaponFragment->WeaponHandType)
			{
			case EWeaponHandType::LeftHand:		EquipHandTypeString = TEXT("Left Hand");	break;
			case EWeaponHandType::RightHand:	EquipHandTypeString = TEXT("Right Hand");	break;
			case EWeaponHandType::TwoHand:		EquipHandTypeString = TEXT("Two Hand");		break;
			}
			Text_WeaponHandType->SetText(FText::FromString(EquipHandTypeString));
			HorizontalBox_WeaponHandType->SetVisibility(ESlateVisibility::Visible);
		}
		else if (EquippableFragment->EquipmentType == EEquipmentType::Utility)
		{
			const UD1ItemFragment_Equippable_Utility* UtilityFragment = Cast<UD1ItemFragment_Equippable_Utility>(EquippableFragment);
			Text_ItemType->SetText(FText::FromString(TEXT("Utility")));

			FString DescriptionString = UtilityFragment->Description.ToString();
			if (UtilityFragment->UtilityType == EUtilityType::Drink)
			{
				if (TSubclassOf<UGameplayEffect> UtilityEffectClass = UtilityFragment->UtilityEffectClass)
				{
					UGameplayEffect* UtilityEffect = UtilityEffectClass->GetDefaultObject<UGameplayEffect>();
					{
						FString A = TEXT("{Period}");
						FString B = FString::SanitizeFloat(UtilityEffect->Period.GetValue());
						DescriptionString = UKismetStringLibrary::Replace(DescriptionString, A, B);
					}
					{
						float Duration = 0.f;
						if (UtilityEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(0.f, Duration))
						{
							FString A = TEXT("{Duration}");
							FString B = FString::SanitizeFloat(Duration);
							DescriptionString = UKismetStringLibrary::Replace(DescriptionString, A, B);
						}
					}
					{
						if (UtilityEffect->Executions.IsValidIndex(0))
						{
							const FGameplayEffectExecutionDefinition& ExecutionDefinitions = UtilityEffect->Executions[0];
							if (ExecutionDefinitions.CalculationModifiers.IsValidIndex(0))
							{
								const FGameplayEffectExecutionScopedModifierInfo& ScopedModifierInfo = ExecutionDefinitions.CalculationModifiers[0];

								float Value = 0.f;
								if (ScopedModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(0.f, Value))
								{
									FString A = TEXT("{Value}");
									FString B = FString::SanitizeFloat(Value);
									DescriptionString = UKismetStringLibrary::Replace(DescriptionString, A, B);
								}
							}
						}
					}
				}
			}
			
			Text_Description->SetText(FText::FromString(DescriptionString));
			Text_Description->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (ItemTemplate.MaxStackCount > 1)
	{
		Text_MaxStackCount->SetText(FText::AsNumber(ItemTemplate.MaxStackCount));
		HorizontalBox_MaxStackCount->SetVisibility(ESlateVisibility::Visible);
	}
}
