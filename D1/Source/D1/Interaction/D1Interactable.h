#pragma once

#include "AbilitySystem/Abilities/D1GameplayAbility.h"
#include "UObject/Interface.h"
#include "D1Interactable.generated.h"

class ID1Interactable;
class UD1GameplayAbility;

USTRUCT(BlueprintType)
struct FD1InteractionInfo
{
	GENERATED_BODY()

public:
	TWeakInterfacePtr<ID1Interactable> Interactable;

	UPROPERTY(EditDefaultsOnly)
	FText InteractionTitle;
	
	UPROPERTY(EditDefaultsOnly)
	FText InteractionContent;

	UPROPERTY(EditDefaultsOnly)
	float HoldTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1GameplayAbility> InteractionAbilityToGrant;

	UPROPERTY()
	FGameplayAbilitySpecHandle InteractionAbilityHandle;
	
public:
	FORCEINLINE bool operator==(const FD1InteractionInfo& Other) const
	{
		return Interactable == Other.Interactable &&
			   InteractionTitle.IdenticalTo(Other.InteractionTitle) &&
			   InteractionContent.IdenticalTo(Other.InteractionContent) &&
			   HoldTime == Other.HoldTime &&
			   InteractionAbilityToGrant == Other.InteractionAbilityToGrant;
	}

	FORCEINLINE bool operator!=(const FD1InteractionInfo& Other) const
	{
		return !operator==(Other);
	}
};

UINTERFACE(MinimalAPI, BlueprintType)
class UD1Interactable : public UInterface
{
	GENERATED_BODY()
};

class ID1Interactable
{
	GENERATED_BODY()

public:
	virtual const FD1InteractionInfo& GetInteractionInfo() const abstract;
};
