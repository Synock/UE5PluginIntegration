// Copyright 2023 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LootableInterface.h"
#include "UObject/Interface.h"
#include "LootableInterfaceExtended.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULootableInterfaceExtended  : public ULootableInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PLUGININTEGRATION_API ILootableInterfaceExtended: public ILootableInterface
{
	GENERATED_BODY()

public:
		virtual void StartLooting(AActor* Looter) override;
};
