#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagStack.generated.h"

USTRUCT(BlueprintType)
struct FGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FGameplayTagStack() { }
	FGameplayTagStack(const FGameplayTag& InTag, int32 InStackCount)
		: Tag(InTag), StackCount(InStackCount) { }

public:
	FString GetDebugString() const;
	
private:
	friend struct FGameplayTagStackContainer;
	
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()
	
public:
	FGameplayTagStackContainer() { }

public:
	void AddStack(const FGameplayTag& Tag, int32 StackCount);
	void RemoveStack(const FGameplayTag& Tag, int32 StackCount);
	
public:
	int32 GetStackCountByTag(const FGameplayTag& Tag) const { return TagToCount.FindRef(Tag); }
	bool ContainsTag(const FGameplayTag& Tag) const { return TagToCount.Contains(Tag); }

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	
private:
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	TMap<FGameplayTag, int32> TagToCount;
};

template<>
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
