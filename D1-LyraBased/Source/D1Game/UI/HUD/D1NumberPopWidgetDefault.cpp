#include "D1NumberPopWidgetDefault.h"

#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1NumberPopWidgetDefault)

UD1NumberPopWidgetDefault::UD1NumberPopWidgetDefault(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1NumberPopWidgetDefault::InitializeUI(int32 InNumberToDisplay, FLinearColor InColor)
{
	Super::InitializeUI(InNumberToDisplay, InColor);

	Text_Number->SetColorAndOpacity(InColor);
	Text_Number->SetText(FText::AsNumber(InNumberToDisplay));

	PlayAnimation(Animation_NumberPop);
}
