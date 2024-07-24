#include "D1CheatListWidget.h"

#include "Components/ScrollBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatListWidget)

UD1CheatListWidget::UD1CheatListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CheatListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ScrollBox_Entries->ClearChildren();
}
