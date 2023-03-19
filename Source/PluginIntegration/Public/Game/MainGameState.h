// Copyright 2023 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/InventoryGameStateInterface.h"
#include "Items/InventoryItemBase.h"
#include "MainGameState.generated.h"

/**
 * 
 */
UCLASS()
class PLUGININTEGRATION_API AMainGameState : public AGameStateBase, public IInventoryGameStateInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<int64, UInventoryItemBase*> ItemMap;

	UPROPERTY(ReplicatedUsing=OnRep_ItemMap, BlueprintReadWrite)
	TArray<UInventoryItemBase*> CompleteItemList;

	UFUNCTION()
	virtual void OnRep_ItemMap();

public:
	UFUNCTION(BlueprintCallable)
	virtual UInventoryItemBase* FetchItemFromID(int32 ID) override;

	UFUNCTION(BlueprintCallable)
	virtual void RegisterItem(UInventoryItemBase* NewItem) override;
	
};
