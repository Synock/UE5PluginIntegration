// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "Interfaces/MerchantInterface.h"
#include "SilentMerchantActor.generated.h"

/// Simple merchant without any interaction related to the dialog plugin.
UCLASS()
class PLUGININTEGRATION_API ASilentMerchantActor : public AActor, public IMerchantInterface, public IInteractableInterface
{
	GENERATED_BODY()

protected:
	//Contains the lootable items.
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory|Merchant")
	TObjectPtr<UMerchantComponent> MerchantComponent;

	//Contains the lootable coins.
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory|Merchant")
	TObjectPtr<UCoinComponent> CashContent_Merchant;

	//Merchant name.
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory|Merchant")
	FString MerchantName = "Merchant";

	//Merchant Sell/Buy ratio. 0.9 means it will buy items at 90% of their price and sell them at 110%
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory|Merchant")
	float MerchantRatio = 0.9f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxInteractionDistance = 500.f;
public:

	// Sets default values for this actor's properties
	ASilentMerchantActor();

	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual UWorld* GetMerchantWorldContext() const override;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual FString GetMerchantName() const override;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual float GetMerchantRatio() const override;

	//Get coin component
	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual UCoinComponent* GetCoinComponent() override;

	//Get merchant component
	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual UMerchantComponent* GetMerchantComponent() override;

	//Get coins
	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual UCoinComponent* GetCoinComponentConst() const override;

	//Get merchant component
	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual UMerchantComponent* GetMerchantComponentConst() const override;

	virtual FOnMerchantDynamicPoolChangedDelegate& GetMerchantDispatcher() const override;

	UFUNCTION(BlueprintCallable)
	virtual void Interact(AActor* Interactor) override;
};
