#pragma once

#include "D1Pickupable.h"
#include "D1WorldInteractable.h"
#include "D1WorldPickupable.generated.h"

UCLASS(Abstract, Blueprintable)
class AD1WorldPickupable : public AActor, public ID1Interactable, public ID1Pickupable
{
	GENERATED_BODY()
	
public:
	AD1WorldPickupable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GatherPostInteractionInfos(const FD1InteractionQuery& InteractionQuery, FD1InteractionInfoBuilder& InteractionInfoBuilder) const override;

public:
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const override { return InteractionInfo; }
	virtual FD1PickupInfo GetPickupInfo() const override { return PickupInfo; }

protected:
	UFUNCTION()
	virtual void OnRep_PickupInfo();
	
protected:
	UPROPERTY(EditAnywhere, Category="Info")
	FD1InteractionInfo InteractionInfo;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_PickupInfo, Category="Info")
	FD1PickupInfo PickupInfo;
};
