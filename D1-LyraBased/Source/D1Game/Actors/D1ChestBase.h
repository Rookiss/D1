#pragma once

#include "Interaction/D1WorldInteractable.h"
#include "D1ChestBase.generated.h"

UENUM(BlueprintType)
enum class EChestState : uint8
{
	Open,
	Close
};

UCLASS(BlueprintType, Blueprintable)
class AD1ChestBase : public AD1WorldInteractable
{
	GENERATED_BODY()
	
public:
	AD1ChestBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const override;

private:
	UFUNCTION()
	void OnRep_ChestState();
	
protected:
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_ChestState)
	EChestState ChestState = EChestState::Close;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Asset_Info")
	FD1InteractionInfo OpenedInteractionInfo;
	
	UPROPERTY(EditDefaultsOnly, Category="Asset_Info")
	FD1InteractionInfo ClosedInteractionInfo;

	UPROPERTY(EditDefaultsOnly, Category="Asset_Info")
	TObjectPtr<UAnimMontage> OpenMontage;

	UPROPERTY(EditDefaultsOnly, Category="Asset_Info")
	TObjectPtr<UAnimMontage> CloseMontage;
};
