#pragma once

#include "UI/D1ActivatableWidget.h"
#include "D1ClassSelectionWidget.generated.h"

class UButton;
class UVerticalBox;
class UD1ClassEntryWidget;

UCLASS()
class UD1ClassSelectionWidget : public UD1ActivatableWidget
{
	GENERATED_BODY()
	
public:
	UD1ClassSelectionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnExitButtonClicked();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1ClassEntryWidget> ClassEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_ClassElements;
};
