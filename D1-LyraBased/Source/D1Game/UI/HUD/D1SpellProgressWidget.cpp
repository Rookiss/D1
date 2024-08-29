#include "D1SpellProgressWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SpellProgressWidget)

UD1SpellProgressWidget::UD1SpellProgressWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SpellProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ConstructListenerHandle = MessageSubsystem.RegisterListener(ConstructMessageChannelTag, this, &ThisClass::ConstructUI);
	RefreshListenerHandle = MessageSubsystem.RegisterListener(RefreshMessageChannelTag, this, &ThisClass::RefreshUI);
}

void UD1SpellProgressWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ConstructListenerHandle);
	MessageSubsystem.UnregisterListener(RefreshListenerHandle);
	
	Super::NativeDestruct();
}

void UD1SpellProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (GetVisibility() == ESlateVisibility::Visible)
	{
		PassedCastTime = FMath::Min(PassedCastTime + InDeltaTime, TargetCastTime);
		ProgressBar_SpellProgress->SetPercent(UKismetMathLibrary::SafeDivide(PassedCastTime, TargetCastTime));
	}
}

void UD1SpellProgressWidget::ConstructUI(FGameplayTag Channel, const FSpellProgressInitializeMessage& Message)
{
	if (Message.bShouldShow)
	{
		PassedCastTime = 0.f;
		TargetCastTime = Message.TotalCastTime;

		Text_SpellName->SetText(Message.DisplayName);
		ProgressBar_SpellProgress->SetPercent(0.f);
		ProgressBar_SpellProgress->SetFillColorAndOpacity(Message.PhaseColor);
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UD1SpellProgressWidget::RefreshUI(FGameplayTag Channel, const FSpellProgressRefreshMessage& Message)
{
	if (GetVisibility() == ESlateVisibility::Visible)
	{
		ProgressBar_SpellProgress->SetFillColorAndOpacity(Message.PhaseColor);
	}
}
