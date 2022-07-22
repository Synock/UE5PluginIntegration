// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/InventoryGameStateInterface.h"
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
	TMap<int64, FInventoryItem> ItemMap;

	UPROPERTY(ReplicatedUsing=OnRep_ItemMap, BlueprintReadWrite)
	TArray<FInventoryItem> CompleteItemList;

	UFUNCTION()
	virtual void OnRep_ItemMap();

public:
	UFUNCTION(BlueprintCallable)
	FInventoryItem FetchItemFromID(int32 ID);

	UFUNCTION(BlueprintCallable)
	void RegisterItem(const FInventoryItem& NewItem);
	
};
