#pragma once

#include "D1UserWidget.h"
#include "D1SpellSelectWidget.generated.h"

struct FAbilityUIInfo;
class USizeBox;
class UTextBlock;
class UImage;
class AD1WeaponBase;

UCLASS()
class UD1SpellSelectWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1SpellSelectWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void HideWidget();

private:
	UFUNCTION()
	void HandleVisibilityChanged(ESlateVisibility InVisibility);
	
	void InitializeUI();
	void RefreshUI();

public:
	UPROPERTY(EditDefaultsOnly)
	int32 SegmentCount = 4;

	UPROPERTY(EditDefaultsOnly)
	float DefaultValueRadius = 100.f;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DefaultColor = FLinearColor(0.185764f, 0.185764f, 0.185764f);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor SelectedColor = FLinearColor(0.067708f, 0.067708f, 0.067708f);

private:
	int32 SelectedIndex = -1;
	int32 EntryCount = 0;
	float SegmentUnitDegree = 0.f;
	float DefaultValueRadiusSquared = 0.f;
	TArray<FAbilityUIInfo> AbilityUIInfos;
	
private:
	UPROPERTY()
	TWeakObjectPtr<AD1WeaponBase> CachedWeaponActor;

	UPROPERTY()
	TArray<TObjectPtr<UImage>> SegmentImages;

	UPROPERTY()
	TArray<TObjectPtr<UImage>> IconImages;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SpellName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Segment_Top;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Segment_Right;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Segment_Down;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Segment_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon_Top;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon_Right;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon_Down;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon_Left;
};
