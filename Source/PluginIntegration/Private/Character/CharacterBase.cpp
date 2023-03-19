// Copyright 2023 Maximilien (Synock) Guislain


#include "Character/CharacterBase.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (HasAuthority())
	{
		if (!Equipment)
		{
			Equipment = CreateDefaultSubobject<UEquipmentComponent>("Equipment");
			Equipment->SetNetAddressable(); // Make DSO components net addressable
			Equipment->SetIsReplicated(true); // Enable replication by default
			Equipment->ItemEquipedDispatcher_Server.AddDynamic(this, &ACharacterBase::HandleEquipmentEffect);
			Equipment->ItemUnEquipedDispatcher_Server.AddDynamic(this, &ACharacterBase::HandleUnEquipmentEffect);
			//Equipment->EquipmentDispatcher_Server.AddDynamic(this, &APlayerCharacter::RecomputeTotalWeight);
		}
	}
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//----------------------------------------------------------------------------------------------------------------------

UEquipmentComponent* ACharacterBase::GetEquipmentComponent()
{
	return Equipment;
}

//----------------------------------------------------------------------------------------------------------------------

const UEquipmentComponent* ACharacterBase::GetEquipmentComponentConst() const
{
	return Equipment;
}

//----------------------------------------------------------------------------------------------------------------------

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, Equipment);
}
