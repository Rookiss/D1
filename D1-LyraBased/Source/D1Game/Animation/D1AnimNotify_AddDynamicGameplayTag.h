#pragma once

#include "GameplayTagContainer.h"
#include "D1AnimNotify_AddDynamicGameplayTag.generated.h"

UCLASS()
class UD1AnimNotify_AddDynamicGameplayTag : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UD1AnimNotify_AddDynamicGameplayTag(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag GameplayTag;
};
