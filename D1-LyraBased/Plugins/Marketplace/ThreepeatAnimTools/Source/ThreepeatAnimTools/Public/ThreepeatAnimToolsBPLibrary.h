// Copyright Threepeat LLC, 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LevelSequence.h"
#include "LevelSequenceEditorToolkit.h"
//#include "ISequencer.h"
#include "UObject/ObjectMacros.h"
#include "MovieSceneBindingProxy.h"

#include "ThreepeatAnimToolsBPLibrary.generated.h"


class ULevelSequence;
class FAssetEditorManager;

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UThreepeatAnimToolsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/*UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clipboard stuff", Keywords = "ThreepeatAnimTools sample test testing"), Category = "ThreepeatAnimToolsTesting")
	static FString ThreepeatAnimToolsClipboardThing(float Param);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clipboard Insert", Keywords = "ThreepeatAnimTools sample test testing"), Category = "ThreepeatAnimToolsTesting")
	static void ThreepeatAnimToolsClipboardInsert(FString contentsToPaste);*/

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Snapshot of Viewer", Keywords = "ThreepeatAnimTools sample test testing"), Category = "ThreepeatAnimToolsTesting")
	static void ThreepeatAnimToolsViewerSnapshot(FString fname);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Selection Set To CSV String", Keywords = "ThreepeatAnimTools selection set csv string"), Category = "ThreepeatAnimTools")
	static FString SelectionSetToString(const TArray<FName>& Controls, const FString& SetName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Euler Filter", Keywords = "ThreepeatAnimTools euler filter"), Category = "ThreepeatAnimTools")
	static float ThreepeatExecuteEulerFilter();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Align to First/Last Key", Keywords = "ThreepeatAnimTools align first last key filter"), Category = "ThreepeatAnimTools")
	static float ThreepeatExecuteAlignToKeyFilter(bool bAlignToFirst = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Shift Keys to Playhead Filter", Keywords = "ThreepeatAnimTools shift key keys filter"), Category = "ThreepeatAnimTools")
	static float ThreepeatExecuteShiftKeysFilter(bool bAlignFirst = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Scroll Sequencer View", Keywords = "ThreepeatAnimTools scroll sequencer top bottom"), Category = "ThreepeatAnimTools")
	static void ThreepeatScrollSequencerToTopOrBottom(bool bScrollToTop = true);


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Time Dilation to selected keys", Keywords = "ThreepeatAnimTools time dilation curveeditor sequencer"), Category = "ThreepeatAnimToolsTesting")
	static float ApplyTimeDilationToSelection();
};
