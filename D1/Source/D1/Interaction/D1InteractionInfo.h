#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "D1InteractionInfo.generated.h"

class ID1Interactable;
class UD1GameplayAbility;

USTRUCT()
struct FD1InteractionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TScriptInterface<ID1Interactable> Interactable;
	
	UPROPERTY(EditAnywhere)
	FText InteractionText;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UD1GameplayAbility> InteractionAbilityToGrant;

public:
	FORCEINLINE bool operator==(const FD1InteractionInfo& Other) const
	{
		return Interactable == Other.Interactable &&
			   InteractionAbilityToGrant == Other.InteractionAbilityToGrant &&
			   InteractionText.IdenticalTo(Other.InteractionText);
	}

	FORCEINLINE bool operator!=(const FD1InteractionInfo& Other) const
	{
		return !operator==(Other);
	}
};
