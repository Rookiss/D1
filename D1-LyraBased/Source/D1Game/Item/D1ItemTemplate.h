#pragma once

#include "D1ItemTemplate.generated.h"

/* TODO:
	*unarmed 착용 시점 문제
	Anim Sync L/R 맞추기
	인벤토리 Contract 구현
	ItemInstance의 주소값을 복제하지 않고 값을 복제(deep copy)하는 방향으로 제작
	Equipment와 Inventory는 ItemInstance 공유 / Equip은 EquipInstance로 따로 분리

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Change : %d"), ChangedIndices.Num()));
*/

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UD1ItemFragment : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void OnInstanceCreated(UD1ItemInstance* Instance) const { }
};

UCLASS(Blueprintable, Const, Abstract)
class UD1ItemTemplate : public UObject
{
	GENERATED_BODY()
	
public:
	UD1ItemTemplate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType="FragmentClass"))
	const UD1ItemFragment* FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}
	
public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint SlotCount = FIntPoint::ZeroValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UD1ItemFragment>> Fragments;
};
