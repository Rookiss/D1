#pragma once

#include "D1Define.h"
#include "Blueprint/UserWidget.h"
#include "D1ClassEntryWidget.generated.h"

class UImage;
class UButton;
class UTextBlock;
class UVerticalBox;
class UD1ClassSelectionWidget;
class UD1ClassSkillEntryWidget;

UCLASS()
class UD1ClassEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ClassEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION()
	void InitializeUI(UD1ClassSelectionWidget* OwnerWidget, ECharacterClassType ClassType);

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
	ECharacterClassType CachedClassType = ECharacterClassType::Count;

	UPROPERTY()
	TWeakObjectPtr<UD1ClassSelectionWidget> CachedOwnerWidget;
};
