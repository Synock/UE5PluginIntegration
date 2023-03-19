// Copyright 2023 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LootableInterfaceExtended.h"
#include "LootableActor.generated.h"

UCLASS()
class PLUGININTEGRATION_API ALootableActor : public AActor, public ILootableInterfaceExtended
{
	GENERATED_BODY()

public:
	ALootableActor();

protected:
	virtual void BeginPlay() override;

	//Contains the lootable items.
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory|Lootable")
	TObjectPtr<ULootPoolComponent> LootPool;

	//Contains the lootable coins.
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory|Lootable")
	TObjectPtr<UCoinComponent> CoinContent;

	//Actor's name
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory|Lootable")
	FString ActorName;

	//Boolean to know if the actor is being looted, as only one person can loot items at a time.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Lootable")
	bool IsBeingLooted = false;

	//Boolean to know if the actor can be destroyed when its loot content is empty.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Lootable")
	bool CanBeDestroyed = false;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BlueprintableStartLoot();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BlueprintableStopLoot();

public:

	virtual UCoinComponent* GetCoinComponent() override;
	virtual ULootPoolComponent* GetLootPoolComponent() override;
	virtual UCoinComponent* GetCoinComponentConst() const override;
	virtual ULootPoolComponent* GetLootPoolComponentConst() const override;

	virtual bool GetIsBeingLooted() const override;
	virtual void SetIsBeingLooted(bool LootStatus) override;

	virtual bool GetIsDestroyable() const override;
	virtual void DestroyLootActor() override;
	virtual FString GetLootActorName() const override;

	virtual void StartLooting(AActor* Looter) override;
	virtual void StopLooting(AActor* Looter) override;
};
