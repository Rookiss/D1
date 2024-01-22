#include "D1GameplayTagStack.h"

#include "D1LogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayTagStack)

FString FD1GameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("[%s x %d]"), *Tag.ToString(), StackCount);
}

bool FD1GameplayTagStackContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FD1GameplayTagStack, FD1GameplayTagStackContainer>(Stacks, DeltaParams, *this);
}

void FD1GameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag& Tag = Stacks[Index].Tag;
		TagToCount.Remove(Tag);
	}
}

void FD1GameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FD1GameplayTagStack& Stack = Stacks[Index];
		TagToCount.Add(Stack.Tag, Stack.StackCount);
	}
}

void FD1GameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FD1GameplayTagStack& Stack = Stacks[Index];
		TagToCount[Stack.Tag] = Stack.StackCount;
	}
}

void FD1GameplayTagStackContainer::AddStack(const FGameplayTag& Tag, int32 StackCount)
{
	if (Tag.IsValid() == false)
	{
		UE_LOG(LogD1AbilitySystem, Error, TEXT("Invalid tag[%s] was passed to AddStack"), *Tag.ToString());
		return;
	}

	if (StackCount > 0)
	{
		for (FD1GameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCount[Tag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}
		FD1GameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
		MarkItemDirty(NewStack);
		TagToCount.Add(Tag, StackCount);
	}
}

void FD1GameplayTagStackContainer::RemoveStack(const FGameplayTag& Tag, int32 StackCount)
{
	if (Tag.IsValid() == false)
	{
		UE_LOG(LogD1AbilitySystem, Error, TEXT("Invalid Tag[%s] was passed to RemoveStack"), *Tag.ToString());
		return;
	}

	if (StackCount > 0)
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FD1GameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				if (Stack.StackCount > StackCount)
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
					TagToCount[Tag] = NewCount;
					MarkItemDirty(Stack);
				}
				else
				{
					It.RemoveCurrent();
					TagToCount.Remove(Tag);
					MarkArrayDirty();
					
					if (Stack.StackCount < StackCount)
					{
						UE_LOG(LogD1AbilitySystem, Error, TEXT("Remove value[%d] is smaller than currently have[%d]"), StackCount, Stack.StackCount);
					}
				}
				return;
			}
		}
		UE_LOG(LogD1AbilitySystem, Error, TEXT("Can't find Stack for Tag[%s]"), *Tag.ToString());
	}
}

FString FD1GameplayTagStackContainer::GetDebugString() const
{
	FString Result;
	for (const auto& Pair : TagToCount)
	{
		Result.Append(FString::Printf(TEXT("(%s:%d)"), *Pair.Key.ToString(), Pair.Value));
	}
	return Result;
}
