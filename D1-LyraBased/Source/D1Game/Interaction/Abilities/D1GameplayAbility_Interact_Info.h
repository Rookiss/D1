#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Interaction/D1InteractionInfo.h"
#include "D1GameplayAbility_Interact_Info.generated.h"

UCLASS()
class UD1GameplayAbility_Interact_Info : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Interact_Info(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	bool InitializeAbility(AActor* TargetActor);

protected:
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ID1Interactable> Interactable;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> InteractableActor;
	
	UPROPERTY(BlueprintReadOnly)
	FD1InteractionInfo InteractionInfo;
};
