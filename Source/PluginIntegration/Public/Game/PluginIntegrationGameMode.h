// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/DialogGameModeInterface.h"
#include "Interfaces/GameModeChatInterface.h"
#include "Interfaces/InventoryGameModeInterface.h"
#include "PluginIntegrationGameMode.generated.h"

UCLASS(minimalapi)
class APluginIntegrationGameMode : public AGameModeBase, public IInventoryGameModeInterface,
                                   public IGameModeChatInterface, public IDialogGameModeInterface
{
	GENERATED_BODY()

protected:
	//------------------------------------------------------------------------------------------------------------------
	// Items
	//------------------------------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite, Category = "Inventory|Item")
	TMap<int32, UInventoryItemBase*> ItemMap;

	//------------------------------------------------------------------------------------------------------------------
	// Dialogs
	//------------------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadWrite, Category = "Dialog")
	TObjectPtr<UDialogMainComponent> DialogComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	TObjectPtr<UQuestMainComponent> QuestComponent;

public:
	APluginIntegrationGameMode();

	//------------------------------------------------------------------------------------------------------------------
	// Inventory
	//------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	virtual UInventoryItemBase* FetchItemFromID(int32 ID) override;

	UFUNCTION(BlueprintCallable)
	virtual void RegisterItem(UInventoryItemBase* NewItem) override;

	// This is a placeholder function that would need to be reimplemented
	virtual ADroppedItem* SpawnItemFromActor(AActor* SpawningActor, uint32 ItemID,
	                                         const FVector& DesiredDropLocation, bool ClampOnGround) override;

	// This is a placeholder function that would need to be reimplemented
	virtual ADroppedCoins* SpawnCoinsFromActor(AActor* SpawningActor, const FCoinValue& CoinValue,
	                                           const FVector& DesiredDropLocation, bool ClampOnGround) override;
	//------------------------------------------------------------------------------------------------------------------
	// Dialogs
	//------------------------------------------------------------------------------------------------------------------

	virtual UDialogMainComponent* GetMainDialogComponent() override { return DialogComponent; }

	virtual UQuestMainComponent* GetMainQuestComponent() override { return QuestComponent; }
};
