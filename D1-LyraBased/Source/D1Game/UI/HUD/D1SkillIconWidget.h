#pragma once

#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Blueprint/UserWidget.h"
#include "D1SkillIconWidget.generated.h"

class UHorizontalBox;
class UImage;
class UTextBlock;

UCLASS()
class UD1SkillIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1SkillIconWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void OnAbilitySystemInitialized();
	
	UFUNCTION()
	void OnAbilityChanged(FGameplayAbilitySpecHandle AbilitySpecHandle, bool bGiven);

	void InitializeUI();
	void RefreshUI();

	FGameplayAbilitySpec* FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle AbilitySpecHandle);
	
protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag SkillSlotTag;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Cooldown;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Cooldown;

private:
	FGameplayAbilitySpecHandle CachedAbilitySpecHandle;
	FDelegateHandle AbilityDelegateHandle;
};
