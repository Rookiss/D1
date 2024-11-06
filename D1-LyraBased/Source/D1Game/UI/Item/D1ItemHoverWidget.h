#pragma once

#include "Blueprint/UserWidget.h"
#include "D1ItemHoverWidget.generated.h"

class UImage;
class UTextBlock;
class UHorizontalBox;
class UD1ItemInstance;

UCLASS()
class UD1ItemHoverWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());\
	
protected:
	virtual void NativePreConstruct() override;

public:
	void RefreshUI(const UD1ItemInstance* ItemInstance);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_DisplayName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_IsEquipped;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemRarity;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AttributeModifiers;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AdditionalAttributeModifiers;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Description;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Gold;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_WeaponHandType;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_DisplayName_Background;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEquippedWidget = false;
};
