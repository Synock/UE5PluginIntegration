// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LootableChest.h"

#include "Character/CharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "Player/PluginIntegrationPlayerController.h"


// Sets default values
ALootableChest::ALootableChest()
{
	PrimaryActorTick.bCanEverTick = false;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMeshComponent"));
	RootComponent = BaseMesh;

	const FAttachmentTransformRules Attachment(EAttachmentRule::KeepRelative, false);
	LidMesh->AttachToComponent(BaseMesh, Attachment);
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableChest::BeginPlay()
{
	Super::BeginPlay();

}

//----------------------------------------------------------------------------------------------------------------------

void ALootableChest::Interact(AActor* Interactor)
{
	const ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(Interactor);
	if (!PlayerCharacter)
		return;

	APluginIntegrationPlayerController* PlayerController = Cast<APluginIntegrationPlayerController>(PlayerCharacter->GetController());
	if (!PlayerController)
		return;

	if (const float Distance = FVector::Distance(GetActorLocation(), Interactor->GetActorLocation()); Distance > MaxInteractionDistance)
		return;

	if (!IsBeingLooted)
	{
		PlayerController->StartLooting(this);
	}
	else
	{
		PlayerController->AddChatData(EChatColor::Red, EMessageCategories::Misc, "Someone is already looting that chest.");
	}

	BlueprintCallableInteraction(Interactor);
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableChest::OnRep_IsOpen()
{
	LidMesh->SetRelativeRotation(IsOpen ? FRotator(0., 0., -45.) : FRotator(0., 0., 0.));
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableChest::StartLooting(AActor* Looter)
{
		Super::StartLooting(Looter);
    	IsOpen = true;
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableChest::StopLooting(AActor* Looter)
{
	Super::StopLooting(Looter);
	IsOpen = false;
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALootableChest, IsOpen);
}
