#pragma once

#include "Interaction/D1WorldInteractable.h"
#include "D1DoorBase.generated.h"

class UArrowComponent;

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	Open_Forward,
	Open_Backward,
	Close
};

UCLASS()
class AD1DoorBase : public AD1WorldInteractable
{
	GENERATED_BODY()
	
public:
	AD1DoorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const override;
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetDoorState(EDoorState NewDoorState);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDoorStateChanged(EDoorState NewDoorState);
	
private:
	UFUNCTION()
	void OnRep_DoorState();

public:
	EDoorState GetDoorState() const { return DoorState; }
	
protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_DoorState)
	EDoorState DoorState = EDoorState::Close;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	FD1InteractionInfo OpenedInteractionInfo;
	
	UPROPERTY(EditDefaultsOnly, Category="Info")
	FD1InteractionInfo ClosedInteractionInfo;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> LeftMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> RightMeshComponent;
};
