#pragma once

#include "UI/D1HoverWidget.h"
#include "D1SkillStatHoverWidget.generated.h"

class UD1SkillStatHoverEntryWidget;

UCLASS()
class UD1SkillStatHoverWidget : public UD1HoverWidget
{
	GENERATED_BODY()
	
public:
	UD1SkillStatHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(FText Name, FText Description);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1SkillStatHoverEntryWidget> Widget_HoverEntry;
};
