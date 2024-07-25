#include "D1CheatEntryWidget.h"

#include "CommonTextBlock.h"
#include "Item/D1ItemTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatEntryWidget)

UD1CheatEntryWidget::UD1CheatEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CheatEntryWidget::InitializeUI(ED1CheatEntryType InCheatEntryType, TSubclassOf<UD1ItemTemplate> InItemTemplateClass, UAnimMontage* InAnimMontage)
{
	switch (InCheatEntryType)
	{
	case ED1CheatEntryType::PrimaryWeapon:
	case ED1CheatEntryType::SecondaryWeapon:
	case ED1CheatEntryType::Armor:
		ItemTemplate = InItemTemplateClass;
		break;
	case ED1CheatEntryType::Animation:
		AnimMontage = InAnimMontage;
		break;
	}

	if (InItemTemplateClass)
	{
		if (UD1ItemTemplate* ItemTemplate = Cast<UD1ItemTemplate>(InItemTemplateClass->GetDefaultObject()))
		{
			Text_Entry->SetText(ItemTemplate->DisplayName);
		}
	}
}
