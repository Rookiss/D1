#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Interaction/D1InteractionInfo.h"
#include "D1GameplayAbility_Interact_Active.generated.h"

class ID1Interactable;

UCLASS()
class UD1GameplayAbility_Interact_Active : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void Initialize(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ID1Interactable> Interactable;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> InteractableActor;
	
	UPROPERTY(BlueprintReadOnly)
	FD1InteractionInfo InteractionInfo;
};
