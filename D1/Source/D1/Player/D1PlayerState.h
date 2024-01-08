#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "D1PlayerState.generated.h"

class UD1SecondarySet;
class UD1PrimarySet;
class UD1AbilitySystemComponent;

UCLASS()
class AD1PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UD1AbilitySystemComponent* GetD1AbilitySystemComponent() const;
	const UD1PrimarySet* GetPrimarySet() const;
	const UD1SecondarySet* GetSecondarySet() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1AbilitySystemComponent> D1ASC;
	
	UPROPERTY()
	TObjectPtr<const UD1PrimarySet> PrimarySet;

	UPROPERTY()
	TObjectPtr<const UD1SecondarySet> SecondarySet;
};
