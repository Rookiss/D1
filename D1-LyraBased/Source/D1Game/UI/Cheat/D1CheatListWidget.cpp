#include "D1CheatListWidget.h"

#include "D1CheatEntryWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatListWidget)

UD1CheatListWidget::UD1CheatListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CheatListWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Title->SetText(TitleText);
}

void UD1CheatListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ScrollBox_Entries->ClearChildren();
}

void UD1CheatListWidget::AddEntry(UD1CheatEntryWidget* EntryWidget)
{
	ScrollBox_Entries->AddChild(EntryWidget);
}
