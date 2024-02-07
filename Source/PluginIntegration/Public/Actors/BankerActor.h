// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogActor.h"
#include "BankerActor.generated.h"

UCLASS()
class PLUGININTEGRATION_API ABankerActor : public ADialogActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABankerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
