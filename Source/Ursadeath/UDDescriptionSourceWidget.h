// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDUIDescriptionReceiver.h"
#include "UDDescriptionSourceWidget.generated.h"

class IUDUIDescriptionReceiver;
class UPanelWidget;
struct FUIDescription;

/**
* A base class for widgets meant to feed description text to another widget implementing the IUDDescriptionReciever interface. Currently, descriptions are sent whenever this widget is moused over.
* 
* Description Widgets don't typically have visual designs of their own and are meant to act as a "wrapper" for other User Widgets, which should typically be a visual representation whatever the description's subject is. Child widgets may be added via the ContentPanel value. 
*/
UCLASS(Abstract)
class URSADEATH_API UUDDescriptionSourceWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** A panel widget meant to hold this classes child widget(s). Widgets set as this panel's child shoul typically be a visual representation of the description's subject.*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> ContentPanel;

	FUIDescription Description;

public:
	/** What the description source will send its description to.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TScriptInterface<IUDUIDescriptionReceiver> DescriptionReceiver;
	

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	/** Set the description that this widget send to be displayed.*/
	void SetDescription(FUIDescription NewDescription);

	/** Returns a panel widget which can be used to parent this widget to other widgets.*/
	UPanelWidget* const GetContentPanel();
};
