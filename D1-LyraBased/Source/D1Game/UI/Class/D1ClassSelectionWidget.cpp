#include "D1ClassSelectionWidget.h"

#include "D1ClassEntryWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Data/D1ClassData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassSelectionWidget)

UD1ClassSelectionWidget::UD1ClassSelectionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ClassSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Exit->OnClicked.AddUniqueDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UD1ClassSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VerticalBox_ClassElements->ClearChildren();

	const TArray<FClassEntry>& ClassEntries = UD1ClassData::Get().GetClassEntries();
	for (const FClassEntry& ClassEntry : ClassEntries)
	{
		UD1ClassEntryWidget* ClassEntryWidget = CreateWidget<UD1ClassEntryWidget>(this, ClassEntryWidgetClass);
		ClassEntryWidget->InitializeUI(ClassEntry);
		VerticalBox_ClassElements->AddChild(ClassEntryWidget);
	}
}

void UD1ClassSelectionWidget::OnExitButtonClicked()
{
	DeactivateWidget();
}
