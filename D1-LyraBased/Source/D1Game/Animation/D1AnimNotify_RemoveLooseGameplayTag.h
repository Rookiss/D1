#pragma once

#include "GameplayTagContainer.h"
#include "D1AnimNotify_RemoveLooseGameplayTag.generated.h"

UCLASS()
class UD1AnimNotify_RemoveLooseGameplayTag : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UD1AnimNotify_RemoveLooseGameplayTag(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag GameplayTag;
	
	UPROPERTY(EditAnywhere)
	bool bShouldReplicate = true;
};
