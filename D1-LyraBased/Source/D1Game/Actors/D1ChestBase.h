#pragma once

#include "Interaction2/D1WorldInteractable.h"
#include "D1ChestBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AD1ChestBase : public AD1WorldInteractable
{
	GENERATED_BODY()
	
public:
	AD1ChestBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual FD1InteractionInfo GetInteractionInfo(const FD1InteractionQuery& InteractionQuery) const override;

private:
	UFUNCTION()
	void OnRep_IsOpened();
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsOpened, BlueprintReadWrite)
	bool bIsOpened = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
	
	UPROPERTY(EditDefaultsOnly)
	FD1InteractionInfo OpenedInteractionInfo;
	
	UPROPERTY(EditDefaultsOnly)
	FD1InteractionInfo ClosedInteractionInfo;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> OpenMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> CloseMontage;
};
