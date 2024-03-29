#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "D1PlayerState.generated.h"

class UD1AttributeSet;
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
	UD1AttributeSet* GetAttributeSet() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UD1AttributeSet> AttributeSet;
};
