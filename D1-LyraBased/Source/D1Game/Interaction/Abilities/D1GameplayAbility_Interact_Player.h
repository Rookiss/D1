#pragma once

#include "CommonActivatableWidget.h"
#include "D1GameplayAbility_Interact_Object.h"
#include "D1GameplayAbility_Interact_Player.generated.h"

class UCommonActivatableWidget;

UCLASS()
class UD1GameplayAbility_Interact_Player : public UD1GameplayAbility_Interact_Object
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Player(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UFUNCTION()
	void OnAfterPushWidget(UCommonActivatableWidget* InPushedWidget);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	TSoftClassPtr<UCommonActivatableWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction", meta=(Categories ="UI.Layer"))
	FGameplayTag WidgetLayerTag;

private:
	UPROPERTY()
	TObjectPtr<UCommonActivatableWidget> PushedWidget;
};
