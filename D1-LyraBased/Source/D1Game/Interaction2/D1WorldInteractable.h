#pragma once

#include "D1Interactable.h"
#include "D1WorldInteractable.generated.h"

UCLASS(Abstract, Blueprintable)
class AD1WorldInteractable : public AActor, public ID1Interactable
{
	GENERATED_BODY()
	
public:
	AD1WorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	virtual void GatherInteractionInfos(const FD1InteractionQuery& InteractionQuery, FD1InteractionInfoBuilder& InteractionInfoBuilder) const override;

protected:
	virtual const FD1InteractionInfo& GetInteractionInfo() const override { return InteractionInfo; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Info")
	FD1InteractionInfo InteractionInfo;
};
