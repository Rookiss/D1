#pragma once

#include "Blueprint/UserWidget.h"
#include "D1ItemDropWidget.generated.h"

UCLASS()
class UD1ItemDropWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemDropWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
