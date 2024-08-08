#pragma once

#include "D1PocketWorldAttachment.generated.h"

class UArrowComponent;

UCLASS(BlueprintType, Blueprintable)
class AD1PocketWorldAttachment : public AActor
{
	GENERATED_BODY()
	
public:
	AD1PocketWorldAttachment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
};
