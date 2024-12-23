// Copyright Threepeat LLC, 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Filters/CurveEditorFilterBase.h"
#include "CurveEditorShiftKeysFilter.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Threepeat - Shift Keys to Playhead")
class THREEPEATANIMTOOLS_API UCurveEditorShiftKeysFilter : public UCurveEditorFilterBase
{
	GENERATED_BODY()
	
public:
	UCurveEditorShiftKeysFilter()
	{
		bAlignFirstKeyToPlayhead = true;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAlignFirstKeyToPlayhead;

protected:

	virtual void InitializeFilter_Impl(TSharedRef<FCurveEditor> InCurveEditoro) override;
	virtual void ApplyFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor, const TMap<FCurveModelID, FKeyHandleSet>& InKeysToOperateOn, TMap<FCurveModelID, FKeyHandleSet>& OutKeysToSelect) override;
	virtual bool CanApplyFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor) override;

};
