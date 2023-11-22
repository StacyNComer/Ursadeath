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

void UUDDescriptionSourceWidget::SetDescription(FUIDescription NewDescription)
{
	Description = NewDescription;
}

UPanelWidget* const UUDDescriptionSourceWidget::GetContentPanel()
{
	return ContentPanel;
}

