// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/PluginIntegrationCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

APluginIntegrationCharacter::APluginIntegrationCharacter():ACharacterBase()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;

	if (HasAuthority())
	{
		CoinPurse = CreateDefaultSubobject<UCoinComponent>("Purse");
		CoinPurse->SetNetAddressable(); // Make DSO components net addressable
		CoinPurse->SetIsReplicated(true); // Enable replication by default
		CoinPurse->PurseDispatcher_Server.AddDynamic(this, &APluginIntegrationCharacter::RecomputeTotalWeight);

		Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
		Inventory->SetNetAddressable(); // Make DSO components net addressable
		Inventory->SetIsReplicated(true); // Enable replication by default
		Inventory->FullInventoryDispatcher_Server.AddDynamic(this, &APluginIntegrationCharacter::RecomputeTotalWeight);
	}
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationCharacter::RecomputeTotalWeight()
{
	// Total weight is the sum of equipment, inventory and coins.
	TotalWeight = CoinPurse->GetTotalWeight() + Inventory->GetTotalWeight() + Equipment->GetTotalWeight();
}

//----------------------------------------------------------------------------------------------------------------------

UInventoryComponent* APluginIntegrationCharacter::GetInventoryComponent()
{
	return Inventory;
}

//----------------------------------------------------------------------------------------------------------------------

const UInventoryComponent* APluginIntegrationCharacter::GetInventoryComponentConst() const
{
	return Inventory;
}

//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* APluginIntegrationCharacter::GetPurseComponent()
{
	return CoinPurse;
}

//----------------------------------------------------------------------------------------------------------------------

const UCoinComponent* APluginIntegrationCharacter::GetPurseComponentConst() const
{
	return CoinPurse;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APluginIntegrationCharacter, CoinPurse, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APluginIntegrationCharacter, Inventory, COND_OwnerOnly);
}