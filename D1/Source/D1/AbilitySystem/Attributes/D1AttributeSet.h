#pragma once

#include "GameplayTags.h"
#include "AttributeSet.h"
#include "D1AttributeSet.generated.h"

class UD1AbilitySystemComponent;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)			\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE(FOutOfHealthDelegate);

UCLASS(Abstract)
class UD1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UD1AttributeSet();
	
public:
	TFunction<FGameplayAttribute()> GetAttributeFuncByTag(const FGameplayTag& AttributeTag) const;
	void GetAllAttributeTags(TArray<FGameplayTag>& OutAttributeTags) const;
	UD1AbilitySystemComponent* GetD1AbilitySystemComponent() const;

protected:
	TMap<FGameplayTag, TFunction<FGameplayAttribute()>> TagToAttributeFunc;
};
