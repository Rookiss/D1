#include "D1GameplayTagStack.h"

#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayTagStack)

FString FD1GameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("%sx%d"), *Tag.ToString(), StackCount);
}

void FD1GameplayTagStackContainer::AddOrRemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (Tag.IsValid() == false || StackCount == 0)
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag or count was passed to AddOrRemoveStack"), ELogVerbosity::Warning);
		return;
	}

	(StackCount > 0) ? AddStack(Tag, StackCount) : RemoveStack(Tag, FMath::Abs(StackCount));
}

void FD1GameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (Tag.IsValid() == false || StackCount <= 0)
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag or count was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}
	
	for (FD1GameplayTagStack& Stack : Stacks)
	{
		if (Stack.Tag == Tag)
		{
			const int32 NewCount = Stack.StackCount + StackCount;
			Stack.StackCount = NewCount;
			TagToCountMap[Tag] = NewCount;
			MarkItemDirty(Stack);
			return;
		}
	}

	FD1GameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
	MarkItemDirty(NewStack);
	TagToCountMap.Add(Tag, StackCount);
	TagContainer.AddTag(Tag);
}

void FD1GameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (Tag.IsValid() == false || StackCount <= 0)
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag or count was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}

	for (auto It = Stacks.CreateIterator(); It; ++It)
	{
		FD1GameplayTagStack& Stack = *It;
		if (Stack.Tag == Tag)
		{
			if (Stack.StackCount <= StackCount)
			{
				It.RemoveCurrent();
				TagToCountMap.Remove(Tag);
				TagContainer.RemoveTag(Tag);
				MarkArrayDirty();
			}
			else
			{
				const int32 NewCount = Stack.StackCount - StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				MarkItemDirty(Stack);
			}
			return;
		}
	}
}

void FD1GameplayTagStackContainer::RemoveStack(FGameplayTag Tag)
{
	if (Tag.IsValid() == false)
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}
	
	for (auto It = Stacks.CreateIterator(); It; ++It)
	{
		FD1GameplayTagStack& Stack = *It;
		if (Stack.Tag == Tag)
		{
			It.RemoveCurrent();
			TagToCountMap.Remove(Tag);
			TagContainer.RemoveTag(Tag);
			MarkArrayDirty();
			return;
		}
	}
}

int32 FD1GameplayTagStackContainer::GetStackCount(FGameplayTag Tag) const
{
	return TagToCountMap.FindRef(Tag);
}

bool FD1GameplayTagStackContainer::ContainsTag(FGameplayTag Tag) const
{
	return TagToCountMap.Contains(Tag);
}

void FD1GameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		TagToCountMap.Remove(Tag);
		TagContainer.RemoveTag(Tag);
	}
}

void FD1GameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FD1GameplayTagStack& Stack = Stacks[Index];
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
		TagContainer.AddTag(Stack.Tag);
	}
}

void FD1GameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FD1GameplayTagStack& Stack = Stacks[Index];
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}
