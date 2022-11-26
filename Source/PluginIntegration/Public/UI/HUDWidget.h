// Copyright 2022 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/HUDChatInterface.h"
#include "HUDWidget.generated.h"

/**
 *
 */
UCLASS()
class PLUGININTEGRATION_API UHUDWidget : public UUserWidget, public IHUDChatInterface
{
protected:
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Chat")
	class UChatBoxWidget* ChatBox = nullptr;

public:
	virtual UChatBoxWidget* GetChatBoxWidget() override {return ChatBox;}

};
