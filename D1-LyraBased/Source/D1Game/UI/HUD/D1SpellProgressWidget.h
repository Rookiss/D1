#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "D1SpellProgressWidget.generated.h"

class UTextBlock;
class UProgressBar;

USTRUCT(BlueprintType)
struct FSpellProgressInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool bShouldShow = false;

	UPROPERTY(BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor PhaseColor;

	UPROPERTY(BlueprintReadWrite)
	float TotalCastTime = 0.f;
};

USTRUCT(BlueprintType)
struct FSpellProgressRefreshMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FLinearColor PhaseColor;
};

UCLASS()
class UD1SpellProgressWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1SpellProgressWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	void ConstructUI(FGameplayTag Channel, const FSpellProgressInitializeMessage& Message);
	void RefreshUI(FGameplayTag Channel, const FSpellProgressRefreshMessage& Message);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SpellName;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_SpellProgress;

protected:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag ConstructMessageChannelTag;

	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag RefreshMessageChannelTag;
	
private:
	float PassedCastTime = 0.f;
	float TargetCastTime = 0.f;
	FGameplayMessageListenerHandle ConstructListenerHandle;
	FGameplayMessageListenerHandle RefreshListenerHandle;
};
