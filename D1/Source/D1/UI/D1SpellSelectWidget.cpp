#include "D1SpellSelectWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/Abilities/D1GameplayAbility_Weapon_Spell.h"
#include "Character/D1Player.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/D1AssetManager.h"
#include "Actors/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SpellSelectWidget)

UD1SpellSelectWidget::UD1SpellSelectWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1SpellSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SegmentImages = { Image_Segment_Top, Image_Segment_Right, Image_Segment_Down, Image_Segment_Left };
	IconImages = { Image_Icon_Top, Image_Icon_Right, Image_Icon_Down, Image_Icon_Left };
	
	SegmentUnitDegree = 360.f / SegmentCount;
	DefaultValueRadiusSquared = DefaultValueRadius * DefaultValueRadius;

	Text_SpellName->SetText(FText::GetEmpty());
	
	OnVisibilityChanged.AddUniqueDynamic(this, &ThisClass::HandleVisibilityChanged);
}

FReply UD1SpellSelectWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	
	FVector2D MousePosition = SizeBox_Root->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D WidgetHalfSize = SizeBox_Root->GetDesiredSize() / 2.f;
	MousePosition = MousePosition - WidgetHalfSize;

	float LengthSquared = MousePosition.SquaredLength();
	if (LengthSquared >= DefaultValueRadiusSquared)
	{
		FVector2D Normalized = MousePosition.GetSafeNormal();
		float Atan = UKismetMathLibrary::DegAtan2(Normalized.X * -1.f, Normalized.Y);
		float Degree = Atan + 180.f + SegmentUnitDegree / 2.f;
		if (Degree >= 360.f)
		{
			Degree -= 360.f;
		}

		SelectedIndex = FMath::TruncToInt(Degree / SegmentUnitDegree);
		if (SelectedIndex >= EntryCount)
		{
			SelectedIndex = -1;
		}
	}
	else
	{
		SelectedIndex = -1;
	}

	for (int32 i = 0; i < SegmentImages.Num(); i++)
	{
		UImage* SegmentImage = SegmentImages[i];
		
		if (i == SelectedIndex)
		{
			SegmentImage->SetBrushTintColor(SelectedColor);
		}
		else
		{
			SegmentImage->SetBrushTintColor(DefaultColor);
		}
	}

	RefreshUI();
	
	return FReply::Handled();
}

void UD1SpellSelectWidget::HandleVisibilityChanged(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Visible)
	{
		InitializeUI();
	}
	else if (InVisibility == ESlateVisibility::Hidden)
	{
		for (UImage* SegmentImage : SegmentImages)
		{
			if (SegmentImage)
			{
				SegmentImage->SetBrushTintColor(DefaultColor);
			}
		}
		
		if (SelectedIndex != -1)
		{
			FGameplayEventData Payload;
			Payload.EventMagnitude = SelectedIndex;
			Payload.Target = CachedWeaponActor.Get();
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPlayer(), D1GameplayTags::Event_ChangeSkill, Payload);
		}
	}
}

void UD1SpellSelectWidget::InitializeUI()
{
	AD1Player* PlayerCharacter = Cast<AD1Player>(GetOwningPlayerPawn());
	check(PlayerCharacter);

	UD1EquipManagerComponent* EquipManager = PlayerCharacter->EquipManagerComponent;
	EWeaponEquipState WeaponEquipState = EquipManager->GetCurrentWeaponEquipState();
	const TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();

	AD1WeaponBase* WeaponActor = nullptr;
	
	switch (WeaponEquipState)
	{
	case EWeaponEquipState::Primary:
		WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_RightHand].SpawnedWeaponActor;
		if (WeaponActor == nullptr)
		{
			WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_TwoHand].SpawnedWeaponActor;
		}
		break;
	case EWeaponEquipState::Secondary:
		WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_RightHand].SpawnedWeaponActor;
		if (WeaponActor == nullptr)
		{
			WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_TwoHand].SpawnedWeaponActor;
		}
		break;
	}

	if (WeaponActor == nullptr || CachedWeaponActor.Get() == WeaponActor)
		return;

	CachedWeaponActor = WeaponActor;

	int32 TemplateID = WeaponActor->TemplateID;
	if (TemplateID <= 0)
		return;
	
	const FD1ItemTemplate& ItemTemplate = UD1AssetManager::GetItemTemplate(TemplateID);
	if (const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		if (const UD1AbilitySystemData* AbilitySystemData = WeaponFragment->AbilitySystemData)
		{
			EntryCount = AbilitySystemData->AbilitiesToPending.Num();
			AbilityUIInfos.Reset();
			
			for (const FD1AbilitySystemData_Ability& PendingAbility : AbilitySystemData->AbilitiesToPending)
			{
				if (const UD1GameplayAbility_Weapon_Spell* SpellAbility = GetDefault<UD1GameplayAbility_Weapon_Spell>(PendingAbility.AbilityClass))
				{
					AbilityUIInfos.Add(SpellAbility->AbilityUIInfo);
				}
				else
				{
					UE_LOG(LogD1, Error, TEXT("Pending Ability Must Inherit Spell Ability"));
				}
			}
		}
	}

	Text_SpellName->SetText(FText::GetEmpty());
	
	for (int i = 0; i < IconImages.Num(); i++)
	{
		UImage* IconImage = IconImages[i];
		
		if (AbilityUIInfos.IsValidIndex(i))
		{
			IconImage->SetBrushFromTexture(AbilityUIInfos[i].AbilityIcon, true);
			IconImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UD1SpellSelectWidget::RefreshUI()
{
	if (SelectedIndex == -1)
	{
		Text_SpellName->SetText(FText::GetEmpty());
	}
	else
	{
		Text_SpellName->SetText(AbilityUIInfos[SelectedIndex].AbilityName);
	}
}
