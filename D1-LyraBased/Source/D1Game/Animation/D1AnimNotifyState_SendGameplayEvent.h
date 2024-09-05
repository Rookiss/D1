#pragma once

#include "D1AnimNotify_SendGameplayEvent.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "D1AnimNotifyState_SendGameplayEvent.generated.h"

UCLASS(meta=(DisplayName="Send Gameplay Event State"))
class UD1AnimNotifyState_SendGameplayEvent : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UD1AnimNotifyState_SendGameplayEvent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag BeginEventTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag TickEventTag;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag EndEventTag;

	UPROPERTY(EditAnywhere)
	FGameplayEventData EventData;
};
