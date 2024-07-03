#pragma once

#include "Abilities/GameplayAbility.h"
#include "D1InteractionInfo.generated.h"

class ID1Interactable;

USTRUCT(BlueprintType)
struct FD1InteractionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<ID1Interactable> Interactable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 0.f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

public:
	FORCEINLINE bool operator==(const FD1InteractionInfo& Other) const
	{
		return Interactable == Other.Interactable &&
			Title.IdenticalTo(Other.Title) &&
			Content.IdenticalTo(Other.Content) &&
			Duration == Other.Duration &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant &&
			InteractionWidgetClass == Other.InteractionWidgetClass;
	}

	FORCEINLINE bool operator!=(const FD1InteractionInfo& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FD1InteractionInfo& Other) const
	{
		return Interactable.GetInterface() < Other.Interactable.GetInterface();
	}
};
