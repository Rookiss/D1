#pragma once

#include "D1AnimNotify_SpawnActor.generated.h"

UCLASS()
class UD1AnimNotify_SpawnActor : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UD1AnimNotify_SpawnActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditAnywhere)
	FName SocketName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;
};
