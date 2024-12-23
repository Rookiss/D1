#pragma once

#include "D1Define.h"
#include "System/D1GameplayTagStack.h"
#include "D1ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct FD1ItemRarityProbability
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditAnywhere)
	float Probability = 0;
};

UCLASS(BlueprintType)
class UD1ItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UD1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

public:
	void Init(int32 InItemTemplateID, EItemRarity InItemRarity);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddOrRemoveStatTagStack(FGameplayTag StatTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(FGameplayTag StatTag);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddOrRemoveOwnedTagStack(FGameplayTag OwnedTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveOwnedTagStack(FGameplayTag OwnedTag);

public:
	static EItemRarity DetermineItemRarity(const TArray<FD1ItemRarityProbability>& ItemProbabilities);
	
public:
	UFUNCTION(BlueprintCallable)
	int32 GetItemTemplateID() const { return ItemTemplateID; }
	
	UFUNCTION(BlueprintCallable)
	EItemRarity GetItemRarity() const { return ItemRarity; }
	
	UFUNCTION(BlueprintCallable)
	bool HasStatTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable)
	int32 GetStatCountByTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable)
	const FD1GameplayTagStackContainer& GetStatContainer() const { return StatContainer; }

	UFUNCTION(BlueprintCallable)
	bool HasOwnedTag(FGameplayTag OwnedTag) const;

	UFUNCTION(BlueprintCallable)
	int32 GetOwnedCountByTag(FGameplayTag OwnedTag) const;
	
	UFUNCTION(BlueprintCallable)
	const FD1GameplayTagStackContainer& GetOwnedTagContainer() const { return OwnedTagContainer; }

public:
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
	
private:
	UPROPERTY(Replicated)
	int32 ItemTemplateID = INDEX_NONE;

	UPROPERTY(Replicated)
	EItemRarity ItemRarity = EItemRarity::Poor;
	
	UPROPERTY(Replicated)
	FD1GameplayTagStackContainer StatContainer;

	UPROPERTY(Replicated)
	FD1GameplayTagStackContainer OwnedTagContainer;
};
