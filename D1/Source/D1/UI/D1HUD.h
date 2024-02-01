#pragma once

#include "GameFramework/HUD.h"
#include "D1HUD.generated.h"

class UD1SceneWidget;

UCLASS()
class AD1HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AD1HUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void ShowSceneWidget();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1SceneWidget> SceneWidget;
};
