// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerStatusIcon.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UUDPlayerStatusIcon::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	/*if (CurrentStatusBarTime > 0)
	{
		CurrentStatusBarTime -= InDeltaTime;

		StatusBar->SetPercent(CurrentStatusBarTime / MaxStatusBarTime);

		FNumberFormattingOptions StatusNumberFormat;
		StatusNumberFormat.MinimumFractionalDigits = StatusNumberFormat.MaximumFractionalDigits = 1;

		if (CurrentStatusBarTime <= 0)
		{
			if (bDisappearWhenStatusDepleted)
			{
				SetVisibility(ESlateVisibility::Collapsed);
			}
			else if (bCounterDisplaysStatusTime)
			{
				StatusCounter->SetVisibility(ESlateVisibility::Collapsed);
			}

			if (bCounterDisplaysStatusTime)
			{
				StatusCounter->SetText(FText::AsNumber(CurrentStatusBarTime, &StatusNumberFormat));
			}
		}
		else if (bCounterDisplaysStatusTime)
		{
			StatusCounter->SetText(FText::AsNumber(CurrentStatusBarTime, &StatusNumberFormat));

		}
	}*/
}

void UUDPlayerStatusIcon::StartStatusBarFromTime(float MaxStatusTime, float CurrentStatusTime)
{
	//Make sure that the widget/statusCounter is visible if it was set to disappear.
	if (bDisappearWhenStatusDepleted)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else if (bCounterDisplaysStatusTime)
	{
		StatusCounter->SetVisibility(ESlateVisibility::Visible);
	}

	StatusBar->SetPercent(1);

	//Set both the max and current status bar value to Status time. The status bar will then deplete over time in the widget's tick function.
	MaxStatusBarTime = MaxStatusTime;
	CurrentStatusBarTime = CurrentStatusTime;
}

void UUDPlayerStatusIcon::StartStatusBar(float StatusTime)
{
	StartStatusBarFromTime(StatusTime, StatusTime);
}

void UUDPlayerStatusIcon::DecrementStatusTime(float DeltaTime)
{
	CurrentStatusBarTime -= DeltaTime;

	StatusBar->SetPercent(CurrentStatusBarTime / MaxStatusBarTime);

	FNumberFormattingOptions StatusNumberFormat;
	StatusNumberFormat.MinimumFractionalDigits = StatusNumberFormat.MaximumFractionalDigits = 1;

	if (CurrentStatusBarTime <= 0)
	{
		if (bDisappearWhenStatusDepleted)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (bCounterDisplaysStatusTime)
		{
			StatusCounter->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (bCounterDisplaysStatusTime)
		{
			StatusCounter->SetText(FText::AsNumber(CurrentStatusBarTime, &StatusNumberFormat));
		}
	}
	else if (bCounterDisplaysStatusTime)
	{
		StatusCounter->SetText(FText::AsNumber(CurrentStatusBarTime, &StatusNumberFormat));

	}
}

UProgressBar* const UUDPlayerStatusIcon::GetStatusBar()
{
	return StatusBar;
}

UTextBlock* const UUDPlayerStatusIcon::GetStatusCounter()
{
	return StatusCounter;
}