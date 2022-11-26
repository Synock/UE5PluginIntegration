// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/GameModeChatInterface.h"
#include "Interfaces/InventoryGameModeInterface.h"
#include "PluginIntegrationGameMode.generated.h"

UCLASS(minimalapi)
class APluginIntegrationGameMode : public AGameModeBase, public IInventoryGameModeInterface, public IGameModeChatInterface
{
	GENERATED_BODY()

protected:

	//------------------------------------------------------------------------------------------------------------------
	// Items
	//------------------------------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite, Category = "Inventory|Item")
	TMap<int32, FInventoryItem> ItemMap;

public:
	APluginIntegrationGameMode();

	//------------------------------------------------------------------------------------------------------------------
	// Inventory
	//------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	virtual FInventoryItem FetchItemFromID(int32 ID) override;

	UFUNCTION(BlueprintCallable)
	virtual void RegisterItem(const FInventoryItem& NewItem) override;

};



