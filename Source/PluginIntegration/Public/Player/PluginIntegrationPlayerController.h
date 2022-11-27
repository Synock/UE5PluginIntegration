// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Character/PluginIntegrationCharacter.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/InventoryPlayerInterface.h"
#include "Interfaces/PlayerChatInterface.h"
#include "UI/HUDWidget.h"
#include "PluginIntegrationPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class APluginIntegrationPlayerController : public APlayerController, public IPlayerChatInterface, public IInventoryPlayerInterface
{
	GENERATED_BODY()

public:

	APluginIntegrationPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	//This is based more or less on the method showed in GASDocumentation (https://github.com/tranek/GASDocumentation)
	void CreateHUD();

protected:

	virtual void OnRep_PlayerState() override;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location);


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UHUDWidget* UIHUDWidget = nullptr;


	//------------------------------------------------------------------------------------------------------------------
	// Chat
	//------------------------------------------------------------------------------------------------------------------

	virtual FString GetChatName() const override;

	virtual IHUDChatInterface* GetChatHUD() override;

	UFUNCTION(Client, unreliable, Category = "Chat|Display")
	virtual void Client_AddChatData(EChatColor Color, EMessageCategories Category, const FString& Message) override;

	UFUNCTION(Client, unreliable, Category = "Chat|Display")
	virtual void Client_AddChatDataType(EGlobalMessageType Type, const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_AreaSpeak(const FString& Message, float Range = 500.) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_ShoutSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_OOCSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_GroupSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_RaidSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_GuildSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_AuctionSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_TellSpeak(const FString& TargetName, const FString& Message) override;

	virtual bool IsInGroup() const override;

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddQuestProgressChatData();

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddNewQuestChatData();

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddItemLootNotification(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddItemRewardNotification(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category="Chat")
	void GetMOTD();

	//------------------------------------------------------------------------------------------------------------------
	// Inventory
	//------------------------------------------------------------------------------------------------------------------

	APluginIntegrationCharacter* GetMainPlayerCharacter() const
	{
		return Cast<APluginIntegrationCharacter>(GetCharacter());
	}

	virtual UInventoryComponent* GetInventoryComponent() override;
	virtual const UInventoryComponent* GetInventoryComponentConst() const override;

	virtual UCoinComponent* GetCoinComponent() override;
	virtual const UCoinComponent* GetCoinComponentConst() const override;

	virtual AActor* GetInventoryOwningActor() override;
	virtual AActor const* GetInventoryOwningActorConst() const override;

	virtual bool GetTransactionBoolean() override;
	virtual void SetTransactionBoolean(bool Value) override;

	virtual AActor* GetMerchantActor() override;
	virtual const AActor* GetMerchantActorConst() const override;
	virtual void SetMerchantActor(AActor* Actor) override;

	virtual AActor* GetLootedActor() override;
	virtual const AActor* GetLootedActorConst() const override;
	virtual void SetLootedActor(AActor* Actor) override;

	virtual IInventoryHUDInterface* GetInventoryHUDInterface() override;
	virtual UObject* GetInventoryHUDObject() override;

	virtual UCoinComponent* GetStagingAreaCoin() override;
	virtual UStagingAreaComponent* GetStagingAreaItems() override;

	virtual UCoinComponent* GetBankCoin() const override;
	virtual UBankComponent* GetBankComponent() const override;

	virtual FOnWeightChanged& GetWeightChangedDelegate() override {return WeightDispatcher;}

	UFUNCTION()
	void OnRep_LootedActor();

	UFUNCTION()
	void OnRep_MerchantActor();

protected:
	bool bInputPressed; // Input is bring pressed
	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed

	FString Name = GetName();
	int32 GroupID = -1;

	//------------------------------------------------------------------------------------------------------------------
	// Inventory
	//------------------------------------------------------------------------------------------------------------------

	bool TransactionBoolean = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Staging")
	TObjectPtr<UCoinComponent> StagingAreaCoin;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Staging")
	TObjectPtr<UStagingAreaComponent> StagingAreaItems;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Bank")
	TObjectPtr<UCoinComponent> BankCoin;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Bank")
	TObjectPtr<UBankComponent> BankComponent;

	UPROPERTY(ReplicatedUsing=OnRep_LootedActor, BlueprintReadOnly, Category = "Inventory|Loot")
	AActor* LootedActor = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_MerchantActor, BlueprintReadOnly, Category = "Inventory|Merchant")
	AActor* MerchantActor = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnWeightChanged WeightDispatcher;


	protected:
	//------------------------------------------------------------------------------------------------------------------
	// Loot -- Server
	//------------------------------------------------------------------------------------------------------------------
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Loot")
	virtual void Server_LootActor(AActor* InputLootedActor) override;

	UFUNCTION(Server, Reliable, Category = "Inventory|Loot")
	virtual void Server_StopLooting() override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Loot")
	virtual void Server_PlayerLootItem(int32 InTopLeft, EBagSlot InSlot, int32 InItemId, int32 OutTopLeft) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Loot")
	virtual void Server_PlayerEquipItemFromLoot(int32 InItemId, EEquipmentSlot InSlot, int32 OutTopLeft) override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|Loot")
	virtual void Server_PlayerAutoLootAll() override;

	//------------------------------------------------------------------------------------------------------------------
	// Merchant -- Server
	//------------------------------------------------------------------------------------------------------------------
	UFUNCTION(Server, Reliable, Category = "Inventory|Merchant")
	virtual void Server_MerchantTrade(AActor* InputMerchantActor) override;

	UFUNCTION(Server, Reliable, Category = "Inventory|Merchant")
	virtual void Server_StopMerchantTrade() override;

	//Buy selected stuff from merchant
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Merchant")
	virtual void Server_PlayerBuyFromMerchant(int32 ItemId, const FCoinValue& Price) override;

	//Sell selected stuff to merchant
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Merchant")
	virtual void
	Server_PlayerSellToMerchant(EBagSlot OutSlot, int32 ItemId, int32 TopLeft, const FCoinValue& Price) override;

	//------------------------------------------------------------------------------------------------------------------
	// Inventory -- Server
	//------------------------------------------------------------------------------------------------------------------

	UFUNCTION(Server, Reliable, WithValidation, Category = "Neverquest|Inventory")
	virtual void Server_PlayerMoveItem(int32 InTopLeft, EBagSlot InSlot, int32 InItemId, int32 OutTopLeft,
	                                   EBagSlot OutSlot) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Equipment")
	virtual void
	Server_PlayerUnequipItem(int32 InTopLeft, EBagSlot InSlot, int32 InItemId, EEquipmentSlot OutSlot) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Equipment")
	virtual void Server_PlayerEquipItemFromInventory(int32 InItemId, EEquipmentSlot InSlot, int32 OutTopLeft,
	                                                 EBagSlot OutSlot) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Equipment")
	virtual void Server_PlayerSwapEquipment(int32 DroppedItemId, EEquipmentSlot DroppedInSlot, int32 SwappedItemId,
	                                        EEquipmentSlot DraggedOutSlot) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory")
	virtual void Server_TransferCoinTo(UCoinComponent* GivingComponent, UCoinComponent* ReceivingComponent,
		const FCoinValue& RemovedCoinValue, const FCoinValue& AddedCoinValue) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Neverquest|Inventory")
	virtual void Server_PlayerAutoEquipItem(int32 InTopLeft, EBagSlot InSlot, int32 InItemId) override;

	//------------------------------------------------------------------------------------------------------------------
	// Staging -- Server
	//------------------------------------------------------------------------------------------------------------------

	UFUNCTION(Server, Reliable, Category = "Inventory")
	virtual void Server_CancelStagingArea() override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory")
	virtual void Server_TransferStagingToActor(AActor* TargetActor) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Staging")
	virtual void Server_MoveEquipmentToStagingArea(int32 InItemId, EEquipmentSlot OutSlot) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory|Staging")
	virtual void Server_MoveInventoryItemToStagingArea(int32 InItemId, int32 OutTopLeft, EBagSlot OutSlot) override;

};


