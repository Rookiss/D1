#include "D1SkillIconWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SkillIconWidget)

UD1SkillIconWidget::UD1SkillIconWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SkillIconWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetVisibility(ESlateVisibility::Hidden);
	
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		for (const FGameplayAbilitySpec& AbilitySpec : LyraASC->GetActivatableAbilities())
		{
			if (AbilitySpec.Ability->AbilityTags.HasTagExact(SkillSlotTag))
			{
				AbilitySpecHandle = AbilitySpec.Handle;
				SetVisibility(ESlateVisibility::Visible);
				break;
			}
		}
	}
}

void UD1SkillIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ULyraPawnExtensionComponent* PawnExtensionComponent = ULyraPawnExtensionComponent::FindPawnExtensionComponent(GetOwningPlayerPawn()))
	{
		PawnExtensionComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	}
}

void UD1SkillIconWidget::NativeDestruct()
{
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		LyraASC->AbilityChangedDelegate.Remove(AbilityDelegateHandle);
	}
	
	Super::NativeDestruct();
}

void UD1SkillIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()))
	{
		if (FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			float CooldownTime = AbilitySpec->Ability->GetCooldownTimeRemaining();
			Text_Cooldown->SetVisibility(CooldownTime > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			Text_Cooldown->SetText(FText::AsNumber(CooldownTime));
		}
	}
}

void UD1SkillIconWidget::OnAbilitySystemInitialized()
{
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		AbilityDelegateHandle = LyraASC->AbilityChangedDelegate.AddUObject(this, &ThisClass::OnAbilityChanged);
	}
}

void UD1SkillIconWidget::OnAbilityChanged(UGameplayAbility* GameplayAbility, bool bGiven)
{
	if (GameplayAbility && GameplayAbility->AbilityTags.HasTagExact(SkillSlotTag))
	{
		if (bGiven)
		{
			SetVisibility(ESlateVisibility::Visible);
			AbilitySpecHandle = GameplayAbility->GetCurrentAbilitySpecHandle();
		}
		else
		{
			SetVisibility(ESlateVisibility::Hidden);
			AbilitySpecHandle = FGameplayAbilitySpecHandle();
		}
	}
}
