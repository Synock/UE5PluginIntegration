// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/PluginIntegrationGameMode.h"

#include "Actors/DroppedCoins.h"
#include "Actors/DroppedItem.h"
#include "Player/PluginIntegrationPlayerController.h"
#include "Character/PluginIntegrationCharacter.h"
#include "Game/MainGameState.h"
#include "UObject/ConstructorHelpers.h"

APluginIntegrationGameMode::APluginIntegrationGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APluginIntegrationPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	if (HasAuthority())
	{
		DialogComponent = CreateDefaultSubobject<UDialogMainComponent>("DialogMainComponent");
		QuestComponent = CreateDefaultSubobject<UQuestMainComponent>("QuestMainComponent");
	}
}

//----------------------------------------------------------------------------------------------------------------------

UInventoryItemBase* APluginIntegrationGameMode::FetchItemFromID(int32 ID)
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

void APluginIntegrationGameMode::RegisterItem(UInventoryItemBase* NewItem)
{
	ItemMap.Add(NewItem->ItemID, NewItem);

	AMainGameState* GS = GetWorld()->GetGameState<AMainGameState>();

	if (GS)
	{
		GS->RegisterItem(NewItem);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot register item as MGS is invalid"));
	}
}

//----------------------------------------------------------------------------------------------------------------------

ADroppedItem* APluginIntegrationGameMode::SpawnItemFromActor(AActor* SpawningActor, uint32 ItemID,
	const FVector& DesiredDropLocation, bool ClampOnGround)
{
	if (!SpawningActor)
		return nullptr;

	if (ItemID <= 0)
		return nullptr;

	const FVector SpawnLocation = GetItemSpawnLocation(SpawningActor, DesiredDropLocation, ClampOnGround);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UInventoryItemBase* ItemToSpawn = FetchItemFromID(ItemID);

	ADroppedItem* Item = GetWorld()->SpawnActor<ADroppedItem>(SpawnLocation, SpawningActor->GetActorRotation(),
																SpawnParams);
	Item->SetReplicates(true);
	Item->InitializeFromItem(ItemToSpawn);
	return Item;
}

//----------------------------------------------------------------------------------------------------------------------

ADroppedCoins* APluginIntegrationGameMode::SpawnCoinsFromActor(AActor* SpawningActor, const FCoinValue& CoinValue,
	const FVector& DesiredDropLocation, bool ClampOnGround)
{
	if (!SpawningActor)
		return nullptr;

	if (CoinValue.IsEmpty())
		return nullptr;

	const FVector SpawnLocation = GetItemSpawnLocation(SpawningActor, DesiredDropLocation, ClampOnGround);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADroppedCoins* Item = GetWorld()->SpawnActor<ADroppedCoins>(SpawnLocation, SpawningActor->GetActorRotation(),
																  SpawnParams);
	Item->SetReplicates(true);
	Item->InitializeFromCoinValue(CoinValue);

	return Item;
}
