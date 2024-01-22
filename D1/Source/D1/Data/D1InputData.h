#pragma once

#include "GameplayTagContainer.h"
#include "D1InputData.generated.h"

class UInputMappingContext;
class UInputAction;

USTRUCT()
struct FD1InputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;
};

UCLASS(Const)
class UD1InputData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> MappingContext;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FD1InputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly)
	TArray<FD1InputAction> AbilityInputActions;
};
