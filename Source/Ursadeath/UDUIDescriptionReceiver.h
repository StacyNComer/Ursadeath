// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UDUIDescriptionReceiver.generated.h"

USTRUCT(BlueprintType)
struct FUIDescription
{
	GENERATED_BODY()

	/** The "Title" of whatever the description is explaining to the player.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Title;

	/** The "Body" text, which should explain whatever the Title text is refering to.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Body;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUDUIDescriptionReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for widgets meant to display UI descriptions recieved from a Description Source Widget.
 */
class URSADEATH_API IUDUIDescriptionReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Called when new description text is sent for the widget to somehow display.*/
	virtual void ReceiveDescription(FUIDescription Description);

	/** Called when a description source is no longer moused over. Override to create behavior for when no description source is being moused over.*/
	virtual void SetDefaultDescription();
};
