// Copyright 2023 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "Components/EquipmentComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/EquipmentInterface.h"
#include "CharacterBase.generated.h"

UCLASS()
class PLUGININTEGRATION_API ACharacterBase : public ACharacter, public IEquipmentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UEquipmentComponent> Equipment;

public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//------------------------------------------------------------------------------------------------------------------
	// Equipment
	//------------------------------------------------------------------------------------------------------------------

	virtual UEquipmentComponent* GetEquipmentComponent() override;

	virtual const UEquipmentComponent* GetEquipmentComponentConst() const override;
};
