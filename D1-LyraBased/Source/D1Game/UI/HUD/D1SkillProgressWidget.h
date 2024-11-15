#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "D1SkillProgressWidget.generated.h"

class UTextBlock;
class UProgressBar;

USTRUCT(BlueprintType)
struct FSkillProgressInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool bShouldShow = false;

	UPROPERTY(BlueprintReadWrite)
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(BlueprintReadWrite)
	FLinearColor PhaseColor = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite)
	float TotalCastTime = 0.f;
};

USTRUCT(BlueprintType)
struct FSkillProgressRefreshMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FLinearColor PhaseColor = FLinearColor::White;
};

UCLASS()
class UD1SkillProgressWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1SkillProgressWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	void ConstructUI(FGameplayTag Channel, const FSkillProgressInitializeMessage& Message);
	void RefreshUI(FGameplayTag Channel, const FSkillProgressRefreshMessage& Message);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SkillName;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_SkillProgress;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_RemainTime;
	
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
