#pragma once

#include "Blueprint/UserWidget.h"
#include "D1ItemHoverWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class UD1ItemInstance;

UCLASS()
class UD1ItemHoverWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(UD1ItemInstance* ItemInstance);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_DisplayName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AttributeModifiers;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Description;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemType;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_WeaponType;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_WeaponHandType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ArmorType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_MaxStackCount;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_WeaponType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_WeaponHandType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_ArmorType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_MaxStackCount;
};
