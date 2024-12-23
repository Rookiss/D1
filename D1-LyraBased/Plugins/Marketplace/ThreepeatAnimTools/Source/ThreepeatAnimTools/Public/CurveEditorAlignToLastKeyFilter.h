// Copyright Threepeat LLC, 2024. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "CurveEditorTypes.h"
#include "Filters/CurveEditorFilterBase.h"

#include "ICurveEditorToolExtension.h"
#include "Misc/FrameNumber.h"
#include "Templates/SharedPointer.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"


#include "CurveEditorAlignToLastKeyFilter.generated.h"

class FCurveEditor;
class UObject;
struct FCurveModelID;
struct FKeyHandleSet;

/**
 * 
 */
UCLASS(DisplayName="Threepeat - Align to First/Last Key")
class /*GAMEANIMSAMP_MOMATCH_API*/ UCurveEditorAlignToLastKeyFilter : public UCurveEditorFilterBase
{
	GENERATED_BODY()
	
public:
	UCurveEditorAlignToLastKeyFilter()
	{
		bAlignToFirst = false;
		//bUseSlope = true;
	}

	/** If true, Align to selected keys to line up with the first (lowest frame number) key.  Defaults to the last key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAlignToFirst;

	/** If true, take slope of 3rd-out to 2nd-out key and apply that delta to the alignment. */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	//bool bUseSlope;

protected:

	virtual void InitializeFilter_Impl(TSharedRef<FCurveEditor> InCurveEditoro) override;
	virtual void ApplyFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor, const TMap<FCurveModelID, FKeyHandleSet>& InKeysToOperateOn, TMap<FCurveModelID, FKeyHandleSet>& OutKeysToSelect) override;
	virtual bool CanApplyFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor) override;

};
