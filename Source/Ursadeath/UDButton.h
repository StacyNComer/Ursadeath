// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDButton.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUDButtonOnClickedEvent);

/**
 * 
 */
UCLASS(Abstract)
class URSADEATH_API UUDButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The button widget making this widget clickable.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Button;

	/** The TextBlock labeling this button.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText;

public:
	/** Called whenever the button attached to this widget is clicked. Used to expose the contained button's "Onclick" event to other widgets containing this one.*/
	UPROPERTY(BlueprintAssignable)
	FUDButtonOnClickedEvent OnClicked;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bStartButtonDisabled;

private:
	/** Broadcasts the OnClick Event. Used to bind this widget's OnClicked to that of this widget's button.*/
	UFUNCTION()
	void ActivateButton();

protected:
	virtual void NativeOnInitialized() override;

public:
	/** Sets whether or not this button is enabled or not. Disabling the button also changes its appearance*/
	UFUNCTION(BlueprintCallable)
	void SetButtonIsEnabled(bool bButtonEnabled);

	/** Returns the text labelling this button.*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTextBlock* const GetButtonText() const;
};
