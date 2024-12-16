#pragma once

#include "D1Define.h"
#include "Item/D1ItemTemplate.h"
#include "D1ItemFragment_Ammo.generated.h"

UCLASS()
class UD1ItemFragment_Ammo : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Ammo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EAmmoType AmmoType = EAmmoType::Count;
};
