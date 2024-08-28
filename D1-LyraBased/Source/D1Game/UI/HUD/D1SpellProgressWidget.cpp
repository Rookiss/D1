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

void UD1SpellProgressWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ProgressBar_SpellProgress->SetFillColorAndOpacity(DefaultColor);
}

void UD1SpellProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HideWidget();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UD1SpellProgressWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::NativeDestruct();
}

void UD1SpellProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (GetVisibility() == ESlateVisibility::Visible && PassedCastTime < TargetCastTime)
	{
		PassedCastTime = FMath::Min(PassedCastTime + InDeltaTime, TargetCastTime);
		ProgressBar_SpellProgress->SetPercent(UKismetMathLibrary::SafeDivide(PassedCastTime, TargetCastTime));
	}
}

void UD1SpellProgressWidget::ConstructUI(FGameplayTag Channel, const FSpellProgressInitializeMessage& Message)
{
	Message.bShouldShow ? ShowWidget(Message.DisplayName, Message.CastTime) : HideWidget();
}

void UD1SpellProgressWidget::ShowWidget(const FText& DisplayName, float CastTime)
{
	PassedCastTime = 0.f;
	TargetCastTime = CastTime;
	
	Text_SpellName->SetText(DisplayName);
	ProgressBar_SpellProgress->SetPercent(0.f);
	
	ProgressBar_SpellProgress->SetFillColorAndOpacity(DefaultColor);
	SetVisibility(ESlateVisibility::Visible);
}

void UD1SpellProgressWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
