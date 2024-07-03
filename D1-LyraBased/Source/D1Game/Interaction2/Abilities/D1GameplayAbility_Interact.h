#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Interaction2/D1InteractionInfo.h"
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
	bool bShouldActive = false;
	
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

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FD1InteractionInfo> CurrentInteractionInfos;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500.f;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
