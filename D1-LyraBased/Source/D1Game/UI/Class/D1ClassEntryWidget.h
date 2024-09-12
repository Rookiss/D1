#pragma once

#include "Blueprint/UserWidget.h"
#include "Data/D1ClassData.h"
#include "D1ClassEntryWidget.generated.h"

class UD1ClassSelectionWidget;
class UImage;
class UButton;
class UTextBlock;
class UVerticalBox;
class UD1ClassSkillEntryWidget;

UCLASS()
class UD1ClassEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ClassEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION()
	void InitializeUI(UD1ClassSelectionWidget* OwnerWidget, int32 ClassIndex);

private:
	UFUNCTION()
	void OnButtonClicked();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1ClassSkillEntryWidget> SkillEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Class;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ClassName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_SkillElements;

private:
	UPROPERTY()
	int32 CachedClassIndex = INDEX_NONE;

	UPROPERTY()
	TWeakObjectPtr<UD1ClassSelectionWidget> CachedOwnerWidget;
};
