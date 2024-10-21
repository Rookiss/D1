#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1GameplayTagStack.generated.h"

struct FD1GameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

USTRUCT(BlueprintType)
struct FD1GameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FD1GameplayTagStack() { }

	FD1GameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount) { }

public:
	const FGameplayTag& GetStackTag() const { return Tag; }
	int32 GetStackCount() const { return StackCount; }
	
	FString GetDebugString() const;
	
private:
	friend FD1GameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FD1GameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FD1GameplayTagStackContainer() { }

public:
	void AddStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStack(FGameplayTag Tag);

public:
	const TArray<FD1GameplayTagStack>& GetStacks() const { return Stacks; }
	
	int32 GetStackCount(FGameplayTag Tag) const { return TagToCountMap.FindRef(Tag); }
	bool ContainsTag(FGameplayTag Tag) const { return TagToCountMap.Contains(Tag); }
	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FD1GameplayTagStack, FD1GameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FD1GameplayTagStack> Stacks;

	UPROPERTY(NotReplicated)
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FD1GameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FD1GameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
