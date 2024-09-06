#include "D1SkillInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SkillInputWidget)

UD1SkillInputWidget::UD1SkillInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SkillInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UD1SkillInputWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::NativeDestruct();
}

void UD1SkillInputWidget::ConstructUI(FGameplayTag Channel, const FSkillInputInitializeMessage& Message)
{
	Message.bShouldShow ? SetVisibility(ESlateVisibility::Visible) : SetVisibility(ESlateVisibility::Hidden);
}
