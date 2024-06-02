#pragma once

#include "D1Define.h"
#include "System/GameplayTagStack.h"
#include "D1ItemInstance.generated.h"

struct FD1ItemRarityProbability;
class UD1ItemDefinition;

UCLASS(BlueprintType)
class UD1ItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UD1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

private:
	void SetItemDefinition(TSubclassOf<UD1ItemDefinition> InItemDefinition, EItemRarity InItemRarity);
	void SetItemDefinition(TSubclassOf<UD1ItemDefinition> InItemDefinition, const TArray<FD1ItemRarityProbability>& InItemProbabilities);
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddStatTagStack(FGameplayTag StatTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(FGameplayTag StatTag, int32 StackCount);

private:
	static EItemRarity DetermineItemRarity(const TArray<FD1ItemRarityProbability>& ItemProbabilities);
	
public:
	TSubclassOf<UD1ItemDefinition> GetItemDefinition() const { return ItemDefinition; }
	EItemRarity GetItemRarity() const { return ItemRarity; }

	UFUNCTION(BlueprintCallable)
	bool HasStatTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable)
	int32 GetStatCountByTag(FGameplayTag StatTag) const;
	
	const FGameplayTagStackContainer& GetStatContainer() const { return StatContainer; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType="FragmentClass"))
	const UD1ItemFragment* FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const;

	template <typename FragmentClass>
		const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}
	
private:
#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS
	
	friend struct FD1InventoryList;
	
private:
	UPROPERTY(Replicated)
	TSubclassOf<UD1ItemDefinition> ItemDefinition;

	UPROPERTY(Replicated)
	EItemRarity ItemRarity = EItemRarity::Junk;
	
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatContainer;
};
