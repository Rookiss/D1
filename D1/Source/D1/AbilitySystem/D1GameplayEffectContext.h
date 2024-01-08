#pragma once

#include "GameplayEffectTypes.h"
#include "D1GameplayEffectContext.generated.h"

USTRUCT()
struct FD1GameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	FD1GameplayEffectContext();

public:
	virtual FGameplayEffectContext* Duplicate() const override;
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
};

template<>
struct TStructOpsTypeTraits<FD1GameplayEffectContext> : public TStructOpsTypeTraitsBase2<FD1GameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
