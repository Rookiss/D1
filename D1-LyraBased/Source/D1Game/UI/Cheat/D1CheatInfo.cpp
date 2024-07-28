#include "D1CheatInfo.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "Player/LyraCheatManager.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatInfo)

UD1CheatInfo::UD1CheatInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CheatInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UD1CheatInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ALyraPlayerController* LyraPlayerController = GetOwningPlayer<ALyraPlayerController>())
	{
		if (ULyraCheatManager* LyraCheatManager = Cast<ULyraCheatManager>(LyraPlayerController->CheatManager))
		{
			if (LyraCheatManager->SelectedMontage.IsNull() == false)
			{
				Text_AnimationName->SetText(FText::FromString(LyraCheatManager->SelectedMontage.GetAssetName()));
			}
			else
			{
				Text_AnimationName->SetText(FText::FromString(TEXT("선택된 애니메이션 없음")));
			}

			Text_AnimationSpeed->SetText(UKismetTextLibrary::Conv_DoubleToText(LyraCheatManager->CurrentAnimationSpeed, HalfToEven, false, true, 1, 2, 1, 1));
		}
	}
}
