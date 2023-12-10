// Fill out your copyright notice in the Description page of Project Settings.

#include "UDDescriptionSourceWidget.h"
#include "UDUIDescriptionReceiver.h"

void UUDDescriptionSourceWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (DescriptionReceiver)
	{
		DescriptionReceiver->ReceiveDescription(Description);
	}
}

void UUDDescriptionSourceWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (DescriptionReceiver)
	{
		DescriptionReceiver->SetDefaultDescription();
	}
}


void UUDDescriptionSourceWidget::SetDescription(FUIDescription NewDescription)
{
	Description = NewDescription;
}

FUIDescription UUDDescriptionSourceWidget::GetDescription()
{
	return Description;
}

UPanelWidget* const UUDDescriptionSourceWidget::GetContentPanel()
{
	return ContentPanel;
}

