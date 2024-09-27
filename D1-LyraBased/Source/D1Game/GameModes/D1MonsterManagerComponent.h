#pragma once

#include "Components/GameStateComponent.h"
#include "D1MonsterManagerComponent.generated.h"

class ALyraCharacter;
class ULyraExperienceDefinition;
class ULyraPawnData;
class AAIController;

UCLASS(Blueprintable, Abstract)
class UD1MonsterManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UD1MonsterManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintAuthorityOnly, Category=Gameplay)
	void ServerInitializeMonsters();
	
private:
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Gameplay)
	TSubclassOf<ALyraCharacter> MonsterCharacterBaseClass;

protected:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ALyraCharacter>> SpawnedMonsterList;
};
