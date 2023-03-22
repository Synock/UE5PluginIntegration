// Copyright 2022 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/DialogDisplayInterface.h"
#include "Interfaces/HUDChatInterface.h"
#include "Interfaces/InventoryHUDInterface.h"
#include "HUDWidget.generated.h"

/**
 *
 */
UCLASS()
class PLUGININTEGRATION_API UHUDWidget : public UUserWidget, public IHUDChatInterface, public  IInventoryHUDInterface , public IDialogDisplayInterface
{
protected:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Chat")
	class UChatBoxWidget* ChatBox = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Dialog")
	class UFinalDialogWindow* DialogWindow = nullptr;

public:

	//------------------------------------------------------------------------------------------------------------------
	// Chat
	//------------------------------------------------------------------------------------------------------------------

	virtual UChatBoxWidget* GetChatBoxWidget() override {return ChatBox;}

	//------------------------------------------------------------------------------------------------------------------
	// Inventory
	//------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void HandleBag(EBagSlot InputBagSlot, class UBagWidget* Widget) override;

	//------------------------------------------------------------------------------------------------------------------
	// Dialog
	//------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	UFinalDialogWindow* GetDialogWindowPointer() const;

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	bool IsDialogDisplayed() const;

	UFUNCTION(Client, Unreliable)
	virtual void ForceDisplayTextInDialog(const FString& TextString) override;

};
