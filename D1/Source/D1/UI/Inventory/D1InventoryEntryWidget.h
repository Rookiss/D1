#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventoryEntryWidget.generated.h"

class USizeBox;

UCLASS()
class UD1InventoryEntryWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetEntrySize(const FIntPoint& Size);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Entry;
};
