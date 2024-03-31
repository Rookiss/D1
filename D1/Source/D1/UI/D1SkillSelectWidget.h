#pragma once

#include "D1UserWidget.h"
#include "D1SkillSelectWidget.generated.h"

UCLASS()
class UD1SkillSelectWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1SkillSelectWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OpenWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void CloseWidget();
};
