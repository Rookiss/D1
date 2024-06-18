#include "D1AnimNotify_RemoveLooseGameplayTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotify_RemoveLooseGameplayTag)

UD1AnimNotify_RemoveLooseGameplayTag::UD1AnimNotify_RemoveLooseGameplayTag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1AnimNotify_RemoveLooseGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (GameplayTag.IsValid())
	{
		if (AActor* Actor = MeshComp->GetOwner())
		{
			if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor)))
			{
				if (bShouldReplicate)
				{
					LyraASC->RemoveReplicatedLooseGameplayTag(GameplayTag);
				}
				else
				{
					LyraASC->RemoveLooseGameplayTag(GameplayTag);
				}
			}
		}
	}
}
