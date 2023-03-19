// Copyright 2023 Maximilien (Synock) Guislain


#include "Game/MainGameState.h"

#include "Net/UnrealNetwork.h"

void AMainGameState::OnRep_ItemMap()
{
	ItemMap.Empty();

	for (auto& Item : CompleteItemList)
	{
		ItemMap.Emplace(Item->ItemID, Item);
	}
	UE_LOG(LogTemp, Log, TEXT("Repping item map for %d items"), ItemMap.Num());
}

//----------------------------------------------------------------------------------------------------------------------

UInventoryItemBase* AMainGameState::FetchItemFromID(int32 ID)
{
	if (!ItemMap.Contains(ID))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find any item at ID %d"), ID);
		return {};
	}
	UInventoryItemBase* LocalItem = ItemMap.FindChecked(ID);
	return LocalItem;
}

//----------------------------------------------------------------------------------------------------------------------

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, CompleteItemList);
}

//----------------------------------------------------------------------------------------------------------------------

void AMainGameState::RegisterItem(UInventoryItemBase* NewItem)
{
	if (HasAuthority())
	{
		CompleteItemList.Add(NewItem);
		UE_LOG(LogTemp, Log, TEXT("Adding item %d %s to MGS"), NewItem->ItemID, *NewItem->Name);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to access MGS Item registration"));
	}
}
