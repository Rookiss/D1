#pragma once

#include "Blueprint/UserWidget.h"
#include "D1CheatListWidget.generated.h"

class UTextBlock;
class UD1CheatEntryWidget;
class UScrollBox;

UCLASS()
class UD1CheatListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1CheatListWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void AddEntry(UD1CheatEntryWidget* EntryWidget);

protected:
	UPROPERTY(EditAnywhere)
	FText TitleText;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Entries;
};
