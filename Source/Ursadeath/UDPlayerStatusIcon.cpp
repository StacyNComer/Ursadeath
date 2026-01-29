// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPlayerStatusIcon.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

//TODO: recomment the code because a lot of this functionality has had major changes to it.

void UUDPlayerStatusIcon::NativePreConstruct()
{
	StatusCounterFormat.MinimumFractionalDigits = StatusCounterFormat.MaximumFractionalDigits = StatusCounterMinFractionalDigits;
}

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
	//Make sure that the widget/status Counter is visible if it was set to disappear.
	if (bDisappearWhenStatusDepleted)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else if (bCounterDisplaysStatusTime)
	{
		StatusCounter->SetVisibility(ESlateVisibility::Visible);
	}

	StatusBar->SetPercent(1);

	//Set both the max and current status bar value to Status time. 
	MaxStatusBarTime = MaxStatusTime;
	CurrentStatusBarTime = CurrentStatusTime;

	StatusCounter->SetText(FText::AsNumber(CurrentStatusBarTime, &StatusCounterFormat));
}

void UUDPlayerStatusIcon::StartStatusBar(float StatusTime)
{
	StartStatusBarFromTime(StatusTime, StatusTime);
}

void UUDPlayerStatusIcon::SetStatusValue(float StatusValue)
{
	CurrentStatusBarTime = StatusValue;

	StatusBar->SetPercent(CurrentStatusBarTime / MaxStatusBarTime);

	if (CurrentStatusBarTime <= 0)
	{
		if (bDisappearWhenStatusDepleted)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (bHideCounterWhenStatusDepleted)
		{
			StatusCounter->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (bCounterDisplaysStatusTime)
		{
			StatusCounter->SetText(FText::AsNumber(CurrentStatusBarTime, &StatusCounterFormat));
		}
	}
	else if (bCounterDisplaysStatusTime)
	{
		StatusCounter->SetText(FText::AsNumber(CurrentStatusBarTime, &StatusCounterFormat));
	}
}

void UUDPlayerStatusIcon::AddStatusValue(float ValueAdded)
{
	SetStatusValue(CurrentStatusBarTime + ValueAdded);
}

UProgressBar* const UUDPlayerStatusIcon::GetStatusBar()
{
	return StatusBar;
}

UTextBlock* const UUDPlayerStatusIcon::GetStatusCounter()
{
	return StatusCounter;
}

void UUDPlayerStatusIcon::SetStatusCounterMinFractionalDigits(int32 Digits)
{
	StatusCounterMinFractionalDigits = Digits;

	StatusCounterFormat.MinimumFractionalDigits = StatusCounterFormat.MaximumFractionalDigits = StatusCounterMinFractionalDigits;
}
