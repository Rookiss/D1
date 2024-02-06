#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "D1InteractionInfo.generated.h"

class ID1Interactable;
class UD1GameplayAbility;

USTRUCT(BlueprintType)
struct FD1InteractionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ID1Interactable> Interactable;
	
	UPROPERTY(EditAnywhere)
	FText InteractionText;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UD1GameplayAbility> InteractionAbilityToGrant;

	UPROPERTY()
	FGameplayAbilitySpecHandle InteractionAbilityHandle;
	
public:
	FORCEINLINE bool operator==(const FD1InteractionInfo& Other) const
	{
		return Interactable == Other.Interactable &&
			   InteractionText.IdenticalTo(Other.InteractionText) &&
			   InteractionAbilityToGrant == Other.InteractionAbilityToGrant;
	}

	FORCEINLINE bool operator!=(const FD1InteractionInfo& Other) const
	{
		return !operator==(Other);
	}
};
