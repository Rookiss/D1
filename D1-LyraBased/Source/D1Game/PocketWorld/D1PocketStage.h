#pragma once

#include "D1PocketStage.generated.h"

class UCameraComponent;
class UPocketCapture;

UCLASS(BlueprintType)
class AD1PocketStage : public AActor
{
	GENERATED_BODY()
	
public:
	AD1PocketStage(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PreCaptureDiffuse();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PostCaptureDiffuse();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PreCaptureAlphaMask();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PostCaptureAlphaMask();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPocketCapture* GetPocketCapute() const { return CachedPocketCapture; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UD1CosmeticManagerComponent* GetCosmeticManager() const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPocketCapture> PocketCaptureClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterial> OverrideMaterial;
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> SpawnedCharacter;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> AlphaMaskMaterialActors;

	UPROPERTY()
	TObjectPtr<UPocketCapture> CachedPocketCapture;
	
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> CachedMaterials;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootSceneComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> CharacterSpawnPoint;
};
