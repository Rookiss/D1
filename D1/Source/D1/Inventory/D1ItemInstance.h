#pragma once

#include "System/GameplayTagStack.h"

#include "D1ItemInstance.generated.h"

struct FGameplayTag;
class UD1ItemFragment;

UCLASS(BlueprintType)
class UD1ItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UD1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetItemID(int32 InItemID) { ItemID = InItemID; }
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount);
	
public:
	virtual bool IsSupportedForNetworking() const override { return true; }

	UFUNCTION(BlueprintCallable)
	int32 GetItemID() { return ItemID; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetStackCountByTag(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintCallable)
	bool HasStatTag(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType=FragmentClass))
	const UD1ItemFragment* FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const;
	
	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const { return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass()); }

private:
	UPROPERTY(Replicated)
	int32 ItemID = 0;
	
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;
};
