#pragma once

#include "Blueprint/UserWidget.h"
#include "D1ItemDragWidget.generated.h"

class UTexture2D;
class USizeBox;
class UImage;
class UTextBlock;

UCLASS()
class UD1ItemDragWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemDragWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(const FVector2D& InWidgetSize, UTexture2D* InItemIcon, int32 InItemCount);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count;
};
