#include "D1ChestBase.h"

#include "Components/ArrowComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ChestBase)

FItemAddRule::FItemAddRule()
{
	ItemRarityProbabilities.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < ItemRarityProbabilities.Num(); i++)
	{
		ItemRarityProbabilities[i].Rarity = (EItemRarity)i;
	}
}

AD1ChestBase::AD1ChestBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);
	
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	
	InventoryManager = CreateDefaultSubobject<UD1InventoryManagerComponent>(TEXT("InventoryManager"));
}

void AD1ChestBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		for (int32 i = 0; i < 5; i++)
		{
			for (FItemAddRule& ItemAddRule : ItemAddRules)
			{
				InventoryManager->TryAddItemByProbability(ItemAddRule.ItemTemplateClass, 1, ItemAddRule.ItemRarityProbabilities);
			}

			if (InventoryManager->IsAllEmpty() == false)
				break;
		}
	}
}

void AD1ChestBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ChestState);
}

FD1InteractionInfo AD1ChestBase::GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const
{
	switch (ChestState)
	{
	case EChestState::Open:		return OpenedInteractionInfo;
	case EChestState::Close:	return ClosedInteractionInfo;
	default:					return FD1InteractionInfo();
	}
}

void AD1ChestBase::SetChestState(EChestState NewChestState)
{
	if (NewChestState == ChestState)
		return;

	ChestState = NewChestState;
	
	if (HasAuthority())
	{
		OnRep_ChestState();
	}
}

void AD1ChestBase::OnRep_ChestState()
{
	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		UAnimMontage* SelectedMontage = nullptr;
		
		switch (ChestState)
		{
		case EChestState::Open:		SelectedMontage = OpenMontage;	break;
		case EChestState::Close:	SelectedMontage = CloseMontage; break;
		}

		if (SelectedMontage)
		{
			AnimInstance->Montage_Play(SelectedMontage);
		}
	}
}
