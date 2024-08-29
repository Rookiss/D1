#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "D1SpellInputWidget.generated.h"

USTRUCT(BlueprintType)
struct FSpellInputInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool bShouldShow = false;
};

UCLASS()
class UD1SpellInputWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1SpellInputWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void ConstructUI(FGameplayTag Channel, const FSpellInputInitializeMessage& Message);

protected:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
private:
	FGameplayMessageListenerHandle ListenerHandle;
};
