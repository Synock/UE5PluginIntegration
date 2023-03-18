// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/PluginIntegrationGameMode.h"
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
