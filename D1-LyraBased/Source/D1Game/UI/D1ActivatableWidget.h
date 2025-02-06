#pragma once

#include "CommonActivatableWidget.h"
#include "D1ActivatableWidget.generated.h"

class UInputAction;

UCLASS()
class UD1ActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UD1ActivatableWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> DeactivateInputAction;
};
