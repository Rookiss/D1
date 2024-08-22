#pragma once

#include "Blueprint/UserWidget.h"
#include "D1CheatEntryWidget.generated.h"

class USizeBox;
class UImage;
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
	Utility,
	Animation
};

UCLASS()
class UD1CheatEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1CheatEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	
public:
	void InitializeUI(ED1CheatEntryType InCheatEntryType, TSubclassOf<UD1ItemTemplate> InItemTemplateClass, TSoftObjectPtr<UAnimMontage> InAnimMontage);

private:
	UFUNCTION()
	void OnButtonClicked();
	
public:
	UPROPERTY()
	ED1CheatEntryType CheatEntryType = ED1CheatEntryType::None;

	UPROPERTY()
	TSubclassOf<UD1ItemTemplate> ItemTemplateClass;

	UPROPERTY()
	TSoftObjectPtr<UAnimMontage> AnimMontage;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Entry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Entry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Entry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Entry;
};
