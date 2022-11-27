// Copyright 2022 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/HUDChatInterface.h"
#include "Interfaces/InventoryHUDInterface.h"
#include "HUDWidget.generated.h"

/**
 *
 */
UCLASS()
class PLUGININTEGRATION_API UHUDWidget : public UUserWidget, public IHUDChatInterface, public  IInventoryHUDInterface
{
protected:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Chat")
	class UChatBoxWidget* ChatBox = nullptr;

public:
	virtual UChatBoxWidget* GetChatBoxWidget() override {return ChatBox;}

	//------------------------------------------------------------------------------------------------------------------
	// Inventory
	//------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void HandleBag(EBagSlot InputBagSlot, class UBagWidget* Widget) override;

};
