#pragma once

#include "Blueprint/UserWidget.h"
#include "D1CheatInfo.generated.h"

class UTextBlock;

UCLASS()
class UD1CheatInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1CheatInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AnimationName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AnimationSpeed;
};
