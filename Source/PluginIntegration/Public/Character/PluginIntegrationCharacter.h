// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/PurseInterface.h"
#include "PluginIntegrationCharacter.generated.h"

UCLASS(Blueprintable)
class APluginIntegrationCharacter : public ACharacterBase,
                                    public IInventoryInterface, public IPurseInterface
{
	GENERATED_BODY()

public:
	APluginIntegrationCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

protected:
	//Pointer to the Inventory component.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> Inventory;

	//Pointer to the Coin/Purse component.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UCoinComponent> CoinPurse;

	//Simple example weight management.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	float TotalWeight = 0.f;

	//Weight update function.
	UFUNCTION()
	void RecomputeTotalWeight();

public:
	//------------------------------------------------------------------------------------------------------------------
	// Inventory
	//------------------------------------------------------------------------------------------------------------------

	virtual UInventoryComponent* GetInventoryComponent() override;

	virtual const UInventoryComponent* GetInventoryComponentConst() const override;

	//------------------------------------------------------------------------------------------------------------------
	// Purse
	//------------------------------------------------------------------------------------------------------------------
	virtual UCoinComponent* GetPurseComponent() override;

	virtual const UCoinComponent* GetPurseComponentConst() const override;
};
