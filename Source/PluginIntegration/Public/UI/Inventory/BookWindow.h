// Copyright 2023 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "BookWindow.generated.h"

/**
 *
 */
UCLASS()
class PLUGININTEGRATION_API UBookWindow : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
	UInventoryBookWidget* InnerWidget = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* TitleBlock = nullptr;

	//For some reason, setting up the pointer in the preconstruct do not ensure they are defined just after the ctor.
	UFUNCTION(BlueprintCallable)
	void InitUI(UInventoryBookWidget* TextWidget, UTextBlock* TitleWidget);

public:

	//For some reason, setting up the pointer in the preconstruct do not ensure they are defined just after the ctor.
	//Using that explicit call will probably work idk.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetupUI();

	UFUNCTION(BlueprintCallable)
	void Close();

	UFUNCTION(BlueprintCallable)
	void SetText(const FText& TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void SetTitle(const FText& TitleToDisplay);

};
