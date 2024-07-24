#pragma once

#include "Blueprint/UserWidget.h"
#include "D1CheatEntryWidget.generated.h"

class UD1ItemTemplate;

UENUM(BlueprintType)
enum class ED1CheatEntryType : uint8
{
	None,
	PrimaryWeapon,
	SecondaryWeapon,
	Armor,
	Animation,
	Util
};

UCLASS()
class UD1CheatEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1CheatEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	ED1CheatEntryType CheatEntryType = ED1CheatEntryType::None;

	UPROPERTY()
	TSubclassOf<UD1ItemTemplate> ItemTemplate;

	UPROPERTY()
	TObjectPtr<UAnimMontage> AnimMontage;
};
