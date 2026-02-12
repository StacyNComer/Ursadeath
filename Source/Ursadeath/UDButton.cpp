// Fill out your copyright notice in the Description page of Project Settings.


#include "UDButton.h"
#include "Components/Button.h"

void UUDButton::ActivateButton()
{
	if (OnClicked.IsBound())
	{
		OnClicked.Broadcast();
	}
}

void UUDButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/** Bind this widget's Onclick event to that of the button it contains.*/
	Button->OnClicked.AddDynamic(this, &UUDButton::ActivateButton);

	if (bStartButtonDisabled)
	{
		SetButtonIsEnabled(false);
	}
}

void UUDButton::SetButtonIsEnabled(bool bButtonEnabled)
{
	Button->SetIsEnabled(bButtonEnabled);
}

UTextBlock* const UUDButton::GetButtonText() const
{
	return ButtonText;
}
