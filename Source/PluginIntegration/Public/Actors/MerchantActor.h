// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogActor.h"
#include "Interfaces/MerchantInterface.h"
#include "MerchantActor.generated.h"


UCLASS()
class PLUGININTEGRATION_API AMerchantActor  : public ADialogActor, public IMerchantInterface
{
protected:
	GENERATED_BODY()

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

public:

	AMerchantActor();

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

	//Get coins
	UFUNCTION(BlueprintCallable, Category = "Inventory|Merchant")
	virtual UMerchantComponent* GetMerchantComponentConst() const override;

	virtual FOnMerchantDynamicPoolChangedDelegate& GetMerchantDispatcher() const override;

};
