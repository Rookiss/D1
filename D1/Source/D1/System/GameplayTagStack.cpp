#include "GameplayTagStack.h"

#include "D1LogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStack)

FString FGameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("[%s x %d]"), *Tag.ToString(), StackCount);
}

void FGameplayTagStackContainer::AddStack(const FGameplayTag& Tag, int32 StackCount)
{
	if (Tag.IsValid() == false)
	{
		UE_LOG(LogD1AbilitySystem, Error, TEXT("Invalid tag[%s] was passed to AddStack"), *Tag.ToString());
		return;
	}

	if (StackCount > 0)
	{
		for (FGameplayTagStack& Stack : Stacks)
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
		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
		MarkItemDirty(NewStack);
		TagToCount.Add(Tag, StackCount);
	}
}

void FGameplayTagStackContainer::RemoveStack(const FGameplayTag& Tag, int32 StackCount)
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
			FGameplayTagStack& Stack = *It;
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

bool FGameplayTagStackContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(Stacks, DeltaParams, *this);
}

void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag& Tag = Stacks[Index].Tag;
		TagToCount.Remove(Tag);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		TagToCount.Add(Stack.Tag, Stack.StackCount);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		TagToCount[Stack.Tag] = Stack.StackCount;
	}
}
