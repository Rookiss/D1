#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1GameplayTagStack.generated.h"

USTRUCT(BlueprintType)
struct FD1GameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FD1GameplayTagStack() { }
	FD1GameplayTagStack(const FGameplayTag& InTag, int32 InStackCount)
		: StatTag(InTag), StackCount(InStackCount) { }

public:
	const FGameplayTag& GetStackTag() const { return StatTag; }
	int32 GetStackCount() const { return StackCount; }
	
	FString GetDebugString() const;
	
private:
	friend struct FD1GameplayTagStackContainer;
	
	UPROPERTY()
	FGameplayTag StatTag;

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
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	
public:
	void AddStack(const FGameplayTag& Tag, int32 StackCount);
	void RemoveStack(const FGameplayTag& Tag, int32 StackCount);
	
public:
	const TArray<FD1GameplayTagStack>& GetStacks() const { return Stacks; } 
	int32 GetStackCountByTag(const FGameplayTag& Tag) const { return TagToCount.FindRef(Tag); }
	bool ContainsTag(const FGameplayTag& Tag) const { return TagToCount.Contains(Tag); }

	FString GetDebugString() const;

private:
	UPROPERTY()
	TArray<FD1GameplayTagStack> Stacks;

	TMap<FGameplayTag, int32> TagToCount;
};

template<>
struct TStructOpsTypeTraits<FD1GameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FD1GameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
