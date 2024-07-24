#pragma once

#include "UI/D1ActivatableWidget.h"
#include "D1CheatMenuWidget.generated.h"

class UD1CheatEntryWidget;
class UD1CheatListWidget;

UCLASS()
class UD1CheatMenuWidget : public UD1ActivatableWidget
{
	GENERATED_BODY()
	
public:
	UD1CheatMenuWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1CheatEntryWidget> CheatEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1CheatListWidget> CheatList_PrimaryWeapon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1CheatListWidget> CheatList_SecondaryWeapon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1CheatListWidget> CheatList_Armor;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1CheatListWidget> CheatList_Animation;
};
