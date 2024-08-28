#include "D1SpellInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SpellInputWidget)

UD1SpellInputWidget::UD1SpellInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SpellInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UD1SpellInputWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::NativeDestruct();
}

void UD1SpellInputWidget::ConstructUI(FGameplayTag Channel, const FSpellInputInitializeMessage& Message)
{
	Message.bShouldShow ? SetVisibility(ESlateVisibility::Visible) : SetVisibility(ESlateVisibility::Hidden);
}
