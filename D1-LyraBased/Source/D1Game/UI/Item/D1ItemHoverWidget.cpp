#include "D1ItemHoverWidget.h"

#include "D1ItemEntryWidget.h"
#include "GameplayEffect.h"
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

#define LOCTEXT_NAMESPACE "ItemHoverWidget"

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
	if (ItemInstance == nullptr)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayAnimationForward(Animation_FadeIn);
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	const EItemRarity ItemRarity = ItemInstance->GetItemRarity();

	FColor RarityColor = UD1UIData::Get().GetRarityColor(ItemRarity);
	Text_IsEquipped->SetColorAndOpacity(RarityColor);
	
	// Default Visibility
	Text_WeaponHandType->SetVisibility(ESlateVisibility::Collapsed);
	Text_AttributeModifiers->SetVisibility(ESlateVisibility::Collapsed);
	Text_AdditionalAttributeModifiers->SetVisibility(ESlateVisibility::Collapsed);
	Text_Description->SetVisibility(ESlateVisibility::Collapsed);
	
	// Display Name
	Text_DisplayName->SetText(ItemTemplate.DisplayName);
	Text_DisplayName->SetColorAndOpacity(RarityColor);

	UTexture2D* RarityTexture = UD1UIData::Get().GetHoverRarityTexture(ItemRarity);
	Image_DisplayName_Background->SetBrushFromTexture(RarityTexture);

	// Item Rarity
	FText ItemRarityText;
	switch (ItemRarity)
	{
	case EItemRarity::Junk:			ItemRarityText = LOCTEXT("ItemRarity-Junk",			"Junk");		break;
	case EItemRarity::Poor:			ItemRarityText = LOCTEXT("ItemRarity-Poor",			"Poor");		break;
	case EItemRarity::Normal:		ItemRarityText = LOCTEXT("ItemRarity-Normal",		"Normal");		break;
	case EItemRarity::Special:		ItemRarityText = LOCTEXT("ItemRarity-Special",		"Special");		break;
	case EItemRarity::Rare:			ItemRarityText = LOCTEXT("ItemRarity-Rare",			"Rare");		break;
	case EItemRarity::Epic:			ItemRarityText = LOCTEXT("ItemRarity-Epic",			"Epic");		break;
	case EItemRarity::Legendary:	ItemRarityText = LOCTEXT("ItemRarity-Legendary",	"Legendary");	break;
	}
	Text_ItemRarity->SetText(ItemRarityText);
	Text_ItemRarity->SetColorAndOpacity(RarityColor);

	// Item Specifics
	if (const UD1ItemFragment_Equippable* EquippableFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable>())
	{
		if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			const UD1ItemFragment_Equippable_Armor* ArmorFragment = Cast<UD1ItemFragment_Equippable_Armor>(EquippableFragment);

			// Item Type
			FText ArmorTypeText;
			switch (ArmorFragment->ArmorType)
			{
			case EArmorType::Helmet:	ArmorTypeText = LOCTEXT("ArmorType-Helmet",	"Helmet");	break;
			case EArmorType::Chest:		ArmorTypeText = LOCTEXT("ArmorType-Chest",	"Chest");	break;
			case EArmorType::Legs:		ArmorTypeText = LOCTEXT("ArmorType-Legs",	"Legs");	break;
			case EArmorType::Hands:		ArmorTypeText = LOCTEXT("ArmorType-Hands",	"Hands");	break;
			case EArmorType::Foot:		ArmorTypeText = LOCTEXT("ArmorType-Foot",	"Foot");	break;
			}
			Text_ItemType->SetText(ArmorTypeText);
		}
		else if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			const UD1ItemFragment_Equippable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(EquippableFragment);

			// Item Type
			FText WeaponTypeText;
			switch (WeaponFragment->WeaponType)
			{
			case EWeaponType::OneHandSword:		WeaponTypeText = LOCTEXT("WeaponType-OneHandSword",	"One-Hand Sword");	break;
			case EWeaponType::TwoHandSword:		WeaponTypeText = LOCTEXT("WeaponType-TwoHandSword", "Two-Hand Sword");	break;
			case EWeaponType::GreatSword:		WeaponTypeText = LOCTEXT("WeaponType-GreatSword",	"Great Sword");		break;
			case EWeaponType::Shield:		    WeaponTypeText = LOCTEXT("WeaponType-Shield",		"Shield");			break;
			case EWeaponType::Bow:				WeaponTypeText = LOCTEXT("WeaponType-Bow",			"Bow");				break;
			case EWeaponType::Staff:			WeaponTypeText = LOCTEXT("WeaponType-Staff",		"Staff");			break;
			}
			Text_ItemType->SetText(WeaponTypeText);

			// Weapon Hand Type
			FText WeaponHandTypeText;
			switch (WeaponFragment->WeaponHandType)
			{
			case EWeaponHandType::LeftHand:		WeaponHandTypeText = LOCTEXT("WeaponHandType-LeftHand",		"Left Hand");	break;
			case EWeaponHandType::RightHand:	WeaponHandTypeText = LOCTEXT("WeaponHandType-RightHand",	"Right Hand");	break;
			case EWeaponHandType::TwoHand:		WeaponHandTypeText = LOCTEXT("WeaponHandType-TwoHand",		"Two Hand");	break;
			}
			Text_WeaponHandType->SetText(WeaponHandTypeText);
			Text_WeaponHandType->SetVisibility(ESlateVisibility::Visible);
		}
		else if (EquippableFragment->EquipmentType == EEquipmentType::Utility)
		{
			const UD1ItemFragment_Equippable_Utility* UtilityFragment = Cast<UD1ItemFragment_Equippable_Utility>(EquippableFragment);

			// Item Type
			FText UtilityTypeText;
			switch (UtilityFragment->UtilityType)
			{
			case EUtilityType::Drink:		UtilityTypeText = LOCTEXT("UtilityType-Drink",			"Drink");			break;
			case EUtilityType::LightSource: UtilityTypeText = LOCTEXT("UtilityType-LightSource",	"Light Source");	break;
			}
			Text_ItemType->SetText(UtilityTypeText);

			// Description
			FString DescriptionString = ItemTemplate.Description.ToString();
			if (UtilityFragment->UtilityType == EUtilityType::Drink)
			{
				if (TSubclassOf<UGameplayEffect> UtilityEffectClass = UtilityFragment->UtilityEffectClass)
				{
					UGameplayEffect* UtilityEffect = UtilityEffectClass->GetDefaultObject<UGameplayEffect>();

					// Peiod
					FString PeriodKey = TEXT("{Period}");
					FString PeriodValue = FString::SanitizeFloat(UtilityEffect->Period.GetValue());
					DescriptionString = UKismetStringLibrary::Replace(DescriptionString, PeriodKey, PeriodValue);

					// Duration
					float Duration = 0.f;
					if (UtilityEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(0.f, Duration))
					{
						FString DurationKey = TEXT("{Duration}");
						FString DurationValue = FString::SanitizeFloat(Duration);
						DescriptionString = UKismetStringLibrary::Replace(DescriptionString, DurationKey, DurationValue);
					}

					// Amount
					if (UtilityEffect->Executions.IsValidIndex(0))
					{
						const FGameplayEffectExecutionDefinition& ExecutionDefinitions = UtilityEffect->Executions[0];
						if (ExecutionDefinitions.CalculationModifiers.IsValidIndex(0))
						{
							const FGameplayEffectExecutionScopedModifierInfo& ScopedModifierInfo = ExecutionDefinitions.CalculationModifiers[0];
		
							float Amount = 0.f;
							if (ScopedModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(0.f, Amount))
							{
								FString AmountKey = TEXT("{Value}");
								FString AmountValue = FString::SanitizeFloat(Amount);
								DescriptionString = UKismetStringLibrary::Replace(DescriptionString, AmountKey, AmountValue);
							}
						}
					}
				}
			}
			
			Text_Description->SetText(FText::FromString(DescriptionString));
			Text_Description->SetVisibility(ESlateVisibility::Visible);
		}

		// Attribute Modifiers
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
	}

	if (Text_ItemType->GetText().IsEmpty())
	{
		Text_ItemType->SetText(LOCTEXT("ItemType-Miscellaneous", "Miscellaneous"));
	}
}

#undef LOCTEXT_NAMESPACE
