#pragma once

#include "CommonActivatableWidget.h"
#include "D1ActivatableWidget.generated.h"

UCLASS()
class UD1ActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UD1ActivatableWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	// TODO: Switch to InputAction
	UPROPERTY(EditDefaultsOnly)
	FKey DeactivateKey;
};
