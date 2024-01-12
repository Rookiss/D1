#pragma once

#include "GameplayTagContainer.h"
#include "D1InputData.generated.h"

class UInputMappingContext;
class UInputAction;

USTRUCT(BlueprintType)
struct FD1InputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;
};

UCLASS(BlueprintType, Const)
class UD1InputData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> MappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FD1InputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FD1InputAction> AbilityInputActions;
};
