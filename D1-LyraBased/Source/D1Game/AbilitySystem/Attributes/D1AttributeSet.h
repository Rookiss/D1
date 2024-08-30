#pragma once

#include "AttributeSet.h"
#include "D1AttributeSet.generated.h"

class ULyraAbilitySystemComponent;
struct FGameplayEffectSpec;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)				\
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_SixParams(FLyraAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

UCLASS()
class D1GAME_API UD1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UD1AttributeSet();

public:
	UWorld* GetWorld() const override;
	ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const;
};
