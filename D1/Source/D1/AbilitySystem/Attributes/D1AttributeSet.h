#pragma once

#include "AttributeSet.h"
#include "D1AttributeSet.generated.h"

class UD1AbilitySystemComponent;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)			\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(Abstract)
class UD1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UD1AttributeSet();
	
public:
	UD1AbilitySystemComponent* GetD1AbilitySystemComponent() const;
};
