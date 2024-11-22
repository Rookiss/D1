#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Interaction/D1InteractionInfo.h"
#include "D1GameplayAbility_Interact.generated.h"

USTRUCT(BlueprintType)
struct FD1InteractionMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRefresh = false;

	UPROPERTY(BlueprintReadWrite)
	bool bSwitchActive = false;
	
	UPROPERTY(BlueprintReadWrite)
	FD1InteractionInfo InteractionInfo = FD1InteractionInfo();
};

UCLASS()
class UD1GameplayAbility_Interact : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FD1InteractionInfo>& InteractionInfos);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

private:
	void WaitInputStart();
	
	UFUNCTION()
	void OnInputStart();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FD1InteractionInfo> CurrentInteractionInfos;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	float InteractionTraceRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	float InteractionTraceRate = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	float InteractionScanRange = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	bool bShowTraceDebug = false;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Interaction")
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
