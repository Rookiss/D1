#pragma once

#include "D1UserWidget.h"
#include "D1SceneWidget.generated.h"

UCLASS()
class UD1SceneWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1SceneWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
};
