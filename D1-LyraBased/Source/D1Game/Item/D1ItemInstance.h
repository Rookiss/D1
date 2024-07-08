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
	EItemRarity Rarity = EItemRarity::Junk;

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

private:
	void Init(int32 InItemTemplateID, EItemRarity InItemRarity);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddStatTagStack(FGameplayTag StatTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(FGameplayTag StatTag);

public:
	static EItemRarity DetermineItemRarity(const TArray<FD1ItemRarityProbability>& ItemProbabilities);
	
public:
	int32 GetItemTemplateID() const { return ItemTemplateID; }
	EItemRarity GetItemRarity() const { return ItemRarity; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasStatTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetStatCountByTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FD1GameplayTagStackContainer& GetStatContainer() const { return StatContainer; }

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
	
	friend struct FD1InventoryEntry;
	friend class UD1EquipmentManagerComponent;
	friend class UD1InventoryManagerComponent;
	
private:
	UPROPERTY(Replicated)
	int32 ItemTemplateID = INDEX_NONE;

	UPROPERTY(Replicated)
	EItemRarity ItemRarity = EItemRarity::Junk;
	
	UPROPERTY(Replicated)
	FD1GameplayTagStackContainer StatContainer;
};
