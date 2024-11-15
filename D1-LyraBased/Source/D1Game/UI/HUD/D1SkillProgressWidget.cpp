#include "D1SkillProgressWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SkillProgressWidget)

UD1SkillProgressWidget::UD1SkillProgressWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SkillProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ConstructListenerHandle = MessageSubsystem.RegisterListener(ConstructMessageChannelTag, this, &ThisClass::ConstructUI);
	RefreshListenerHandle = MessageSubsystem.RegisterListener(RefreshMessageChannelTag, this, &ThisClass::RefreshUI);
}

void UD1SkillProgressWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ConstructListenerHandle);
	MessageSubsystem.UnregisterListener(RefreshListenerHandle);
	
	Super::NativeDestruct();
}

void UD1SkillProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (GetVisibility() == ESlateVisibility::Visible)
	{
		PassedCastTime = FMath::Min(PassedCastTime + InDeltaTime, TargetCastTime);
		ProgressBar_SkillProgress->SetPercent(UKismetMathLibrary::SafeDivide(PassedCastTime, TargetCastTime));

		FNumberFormattingOptions Options;
		Options.MinimumFractionalDigits = 1;
		Options.MaximumFractionalDigits = 1;
		Text_RemainTime->SetText(FText::AsNumber(FMath::Clamp(TargetCastTime - PassedCastTime, 0.f, TargetCastTime), &Options));
	}
}

void UD1SkillProgressWidget::ConstructUI(FGameplayTag Channel, const FSkillProgressInitializeMessage& Message)
{
	if (Message.bShouldShow)
	{
		PassedCastTime = 0.f;
		TargetCastTime = Message.TotalCastTime;

		Text_SkillName->SetText(Message.DisplayName);
		ProgressBar_SkillProgress->SetPercent(0.f);
		ProgressBar_SkillProgress->SetFillColorAndOpacity(Message.PhaseColor);
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UD1SkillProgressWidget::RefreshUI(FGameplayTag Channel, const FSkillProgressRefreshMessage& Message)
{
	if (GetVisibility() == ESlateVisibility::Visible)
	{
		ProgressBar_SkillProgress->SetFillColorAndOpacity(Message.PhaseColor);
	}
}
