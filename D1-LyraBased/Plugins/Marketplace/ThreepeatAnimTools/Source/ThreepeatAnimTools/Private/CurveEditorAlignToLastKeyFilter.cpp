// Copyright Threepeat LLC, 2024. All Rights Reserved.

#include "CurveEditorAlignToLastKeyFilter.h"

#include "Containers/Array.h"
#include "Containers/ArrayView.h"
#include "CurveDataAbstraction.h"
#include "CurveEditor.h"
#include "CurveEditorSelection.h"
#include "CurveEditorSnapMetrics.h"
#include "CurveEditorTypes.h"
#include "CurveModel.h"
#include "Curves/KeyHandle.h"
#include "Curves/RealCurve.h"
#include "HAL/PlatformCrt.h"
#include "Math/NumericLimits.h"
#include "Math/UnrealMathSSE.h"
#include "Misc/FrameRate.h"
#include "Misc/Optional.h"
#include "Templates/Tuple.h"
#include "Templates/UnrealTemplate.h"

void UCurveEditorAlignToLastKeyFilter::InitializeFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor)
{
	if (!InCurveEditor->GetTimeSliderController())
	{
		return;
	}

	/*
	// There is a valid time slider controller, so we can use the display rate and tick resolution from it to define frames
	bUseSeconds = false;

	// Initialize Bake Interval and Custom Ranges if they haven't been initialized with these parameters
	const FFrameRate DisplayRate = InCurveEditor->GetTimeSliderController()->GetDisplayRate();
	const FFrameRate TickResolution = InCurveEditor->GetTimeSliderController()->GetTickResolution();

	if (DisplayRate == InitialDisplayRate && TickResolution == InitialTickResolution)
	{
		return;
	}

	InitialDisplayRate = DisplayRate;
	InitialTickResolution = TickResolution;

	TRange<FFrameNumber> PlayRange = InCurveEditor->GetTimeSliderController()->GetPlayRange();

	BakeInterval = FFrameRate::TransformTime(FFrameTime(1), DisplayRate, TickResolution).FrameNumber;
	CustomRange.Min = PlayRange.GetLowerBoundValue();
	CustomRange.Max = PlayRange.GetUpperBoundValue();
	*/
}

bool UCurveEditorAlignToLastKeyFilter::CanApplyFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor)
{
	//return bCustomRangeOverride || 
	return InCurveEditor->GetSelection().Count() > 0;
}

void UCurveEditorAlignToLastKeyFilter::ApplyFilter_Impl(TSharedRef<FCurveEditor> InCurveEditor, const TMap<FCurveModelID, FKeyHandleSet>& InKeysToOperateOn, TMap<FCurveModelID, FKeyHandleSet>& OutKeysToSelect)
{
	TArray<FKeyHandle> KeyHandles;
	TArray<FKeyHandle> KeyHandlesToModify;
	TArray<FKeyPosition> SelectedKeyPositions;

	TArray<FKeyPosition> NewKeyPositions;

	int totalKeys = 0;

	for (const TTuple<FCurveModelID, FKeyHandleSet>& Pair : InKeysToOperateOn)
	{
		totalKeys += Pair.Value.Num();
	}


	UE_LOG(LogTemp, Warning, TEXT("Testing!"));
	UE_LOG(LogTemp, Warning, TEXT("Num curves: %d, num keys total: %d"), InKeysToOperateOn.Num(), totalKeys);


	
	for (const TTuple<FCurveModelID, FKeyHandleSet>& Pair : InKeysToOperateOn)
	{
		// first check if curve exists
		FCurveModel* Curve = InCurveEditor->FindCurve(Pair.Key);
		if (!Curve)
		{
			continue;
		}

		/*// then check if curve is for rotation value
		if (!Curve->GetIntentionName().Contains("Rotation"))
		{
			continue;
		}*/

		KeyHandles.Reset(Pair.Value.Num());
		KeyHandles.Append(Pair.Value.AsArray().GetData(), Pair.Value.Num());

		// Get all the selected keys
		SelectedKeyPositions.SetNum(KeyHandles.Num());
		Curve->GetKeyPositions(KeyHandles, SelectedKeyPositions);

		// Find the hull of the range of the selected keys
		double MinKey = TNumericLimits<double>::Max(), MaxKey = TNumericLimits<double>::Lowest();
		for (FKeyPosition Key : SelectedKeyPositions)
		{
			MinKey = FMath::Min(Key.InputValue, MinKey);
			MaxKey = FMath::Max(Key.InputValue, MaxKey);
		}

		// Get all keys that exist between the time range
		KeyHandles.Reset();
		Curve->GetKeys(*InCurveEditor, MinKey, MaxKey, TNumericLimits<double>::Lowest(), TNumericLimits<double>::Max(), KeyHandles);

		if ((/*!bUseSlope &&*/ (KeyHandles.Num() >= 2)) || (KeyHandles.Num() > 2))
		{
			// get the selected key positions
			SelectedKeyPositions.SetNum(KeyHandles.Num());
			Curve->GetKeyPositions(KeyHandles, SelectedKeyPositions);


			int lastKeyIndex = 0;
			int secondOutIndex = 1;
			int thirdOutIndex = 2;

			
			if (bAlignToFirst) 
			{
				// everything's done already
			}
			else {
				lastKeyIndex = KeyHandles.Num() - 1;
				secondOutIndex = lastKeyIndex - 1;
				thirdOutIndex = lastKeyIndex - 2;
			}


			/*float LastKeyVal = SelectedKeyPositions[KeyHandles.Num() - 1].OutputValue;
			float SecondToLastKeyVal = SelectedKeyPositions[KeyHandles.Num() - 2].OutputValue;*/
			float LastKeyVal = SelectedKeyPositions[lastKeyIndex].OutputValue;
			float SecondToLastKeyVal = SelectedKeyPositions[secondOutIndex].OutputValue;


			float diffToAdd = LastKeyVal - SecondToLastKeyVal;

			UE_LOG(LogTemp, Warning, TEXT("Curves[%s]: secondToLast( %f ), last( %f )"), *(Curve->GetIntentionName()), SecondToLastKeyVal, LastKeyVal);

			// reset the old calculated key positions
			NewKeyPositions.Reset();
			KeyHandlesToModify.Reset();

			for (int32 KeyIndex = 0; KeyIndex <= KeyHandles.Num() - 1; KeyIndex++)
			{
				if (KeyIndex != lastKeyIndex) {
					SelectedKeyPositions[KeyIndex].OutputValue += diffToAdd;
				}

				// create the new key position and add it and its handle to their respective lists
				FKeyPosition FilteredKey(SelectedKeyPositions[KeyIndex].InputValue, SelectedKeyPositions[KeyIndex].OutputValue);
				NewKeyPositions.Add(FilteredKey);
				KeyHandlesToModify.Add(KeyHandles[KeyIndex]);
			}

			Curve->Modify();
			Curve->SetKeyPositions(KeyHandlesToModify, NewKeyPositions);
		}
	}
	OutKeysToSelect = InKeysToOperateOn;
}
