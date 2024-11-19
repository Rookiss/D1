#include "D1ClassSelectionWidget.h"

#include "D1ClassEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Data/D1ClassData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassSelectionWidget)

UD1ClassSelectionWidget::UD1ClassSelectionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ClassSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VerticalBox_ClassElements->ClearChildren();

	const int32 ClassCount = (int32)ECharacterClassType::Count;
	for (int i = 0; i < ClassCount; i++)
	{
		UD1ClassEntryWidget* ClassEntryWidget = CreateWidget<UD1ClassEntryWidget>(this, ClassEntryWidgetClass);
		ClassEntryWidget->InitializeUI(this, (ECharacterClassType)i);
		VerticalBox_ClassElements->AddChild(ClassEntryWidget);
	}
}

void UD1ClassSelectionWidget::OnExitButtonClicked()
{
	DeactivateWidget();
}
