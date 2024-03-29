#pragma once

#include "D1Define.h"
#include "Data/D1ItemData.h"
#include "System/D1AssetManager.h"
#include "System/D1GameplayTagStack.h"
#include "D1ItemInstance.generated.h"

UCLASS(BlueprintType)
class UD1ItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UD1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS
	
public:
	void Init(int32 InTemplateID);
	
	void AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount);
	void RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount);

public:
	int32 GetTemplateID() const { return TemplateID; }
	EItemRarity GetItemRarity() const { return ItemRarity; }

	const FD1GameplayTagStackContainer& GetStatContainer() const { return StatContainer; }
	int32 GetStackCountByTag(const FGameplayTag& StatTag) const;
	bool HasStatTag(const FGameplayTag& StatTag) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const;

private:
	EItemRarity DetermineItemRarity();
	
private:
	UPROPERTY(Replicated)
	int32 TemplateID = 0;

	UPROPERTY(Replicated)
	EItemRarity ItemRarity = EItemRarity::Junk;
	
	UPROPERTY(Replicated)
	FD1GameplayTagStackContainer StatContainer;
};

template <typename FragmentClass>
const FragmentClass* UD1ItemInstance::FindFragmentByClass() const
{
	const FD1ItemTemplate& ItemTemplate = UD1AssetManager::GetItemTemplate(TemplateID);
	return ItemTemplate.FindFragmentByClass<FragmentClass>();
}
