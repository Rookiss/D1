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

#include "CurveEditorMakeHoldFilter.generated.h"


class FCurveEditor;
class UObject;
struct FCurveModelID;
struct FKeyHandleSet;

/**
 * 
 */
UCLASS(DisplayName="Threepeat - Make Hold")
class /*GAMEANIMSAMP_MOMATCH_API*/ UCurveEditorMakeHoldFilter : public UCurveEditorFilterBase
{
	GENERATED_BODY()

public:

	UCurveEditorMakeHoldFilter()
	{
		bHoldOnLast = false;
		bOvershoot = false;
		bOvershootSprintConstant = 100.0f;
		bOvershootDamping = 1.0f;
		bOvershootPrerollHistorySeconds = 0.1;
	}

	/** If true, use the last key as the old value (defaults to using first key value)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bHoldOnLast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bOvershoot = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = "bOvershoot"))
	float bOvershootSprintConstant = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = "bOvershoot"))
	float bOvershootDamping = 5.0f;

	/** how many selected keys before the hold should start (must be > 1 for overshoot to work)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = "bOvershoot"))
	float bOvershootPrerollHistorySeconds = 0.1;

	virtual void ApplyFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor, const TMap<FCurveModelID, FKeyHandleSet>& InKeysToOperateOn, TMap<FCurveModelID, FKeyHandleSet>& OutKeysToSelect) override;
};
