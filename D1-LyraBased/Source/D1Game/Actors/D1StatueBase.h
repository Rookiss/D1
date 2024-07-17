#pragma once

#include "Interaction/D1WorldInteractable.h"
#include "D1StatueBase.generated.h"

class UArrowComponent;

UCLASS()
class AD1StatueBase : public AD1WorldInteractable
{
	GENERATED_BODY()
	
public:
	AD1StatueBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual bool CanInteraction(const FD1InteractionQuery& InteractionQuery) const override;
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const override;
	virtual UMeshComponent* GetInteractionMeshComponent() const override { return MeshComponent; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Info")
	FD1InteractionInfo InteractionInfo;

	UPROPERTY(BlueprintReadWrite)
	bool bIsUsed = false;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
