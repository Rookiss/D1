#pragma once

#include "Blueprint/UserWidget.h"
#include "D1CheatEntryWidget.generated.h"

class UButton;
class UCommonTextBlock;
class UD1ItemTemplate;

UENUM(BlueprintType)
enum class ED1CheatEntryType : uint8
{
	None,
	PrimaryWeapon,
	SecondaryWeapon,
	Armor,
	Animation
};

UCLASS()
class UD1CheatEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1CheatEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeUI(ED1CheatEntryType InCheatEntryType, TSubclassOf<UD1ItemTemplate> InItemTemplateClass, UAnimMontage* InAnimMontage);
	
public:
	UPROPERTY(EditDefaultsOnly)
	ED1CheatEntryType CheatEntryType = ED1CheatEntryType::None;

	UPROPERTY()
	TSubclassOf<UD1ItemTemplate> ItemTemplate;

	UPROPERTY()
	TObjectPtr<UAnimMontage> AnimMontage;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Entry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Entry;
};
