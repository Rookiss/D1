#include "D1SkillIconWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SkillIconWidget)

UD1SkillIconWidget::UD1SkillIconWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SkillIconWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (ASC == nullptr)
		return;
	
	for (const FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
	{
		if (AbilitySpec.Ability->AbilityTags.HasTagExact(SkillSlotTag))
		{
			CachedAbilitySpecHandle = AbilitySpec.Handle;
			SetVisibility(ESlateVisibility::Visible);
			break;
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
		AbilityDelegateHandle.Reset();
	}
	
	Super::NativeDestruct();
}

void UD1SkillIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshUI();
}

void UD1SkillIconWidget::RefreshUI()
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (ASC == nullptr)
		return;

	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(CachedAbilitySpecHandle);
	if (AbilitySpec == nullptr)
		return;
	
	float CooldownTime = AbilitySpec->Ability->GetCooldownTimeRemaining(ASC->AbilityActorInfo.Get());
	if (CooldownTime > 0.f)
	{
		Image_SkillIcon->SetColorAndOpacity(FLinearColor::Gray);
		Text_Cooldown->SetText(UKismetTextLibrary::Conv_DoubleToText(CooldownTime, HalfFromZero, false, true, 1, 2, 1, 1));
		HorizontalBox_Cooldown->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_SkillIcon->SetColorAndOpacity(FLinearColor::White);
		HorizontalBox_Cooldown->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UD1SkillIconWidget::OnAbilitySystemInitialized()
{
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		AbilityDelegateHandle = LyraASC->AbilityChangedDelegate.AddUObject(this, &ThisClass::OnAbilityChanged);
	}
}

void UD1SkillIconWidget::OnAbilityChanged(FGameplayAbilitySpecHandle AbilitySpecHandle, bool bGiven)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (ASC == nullptr)
		return;

	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (AbilitySpec == nullptr)
		return;
	
	if (AbilitySpec->Ability->AbilityTags.HasTagExact(SkillSlotTag))
	{
		if (bGiven)
		{
			CachedAbilitySpecHandle = AbilitySpec->Handle;
			SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CachedAbilitySpecHandle = FGameplayAbilitySpecHandle();
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
