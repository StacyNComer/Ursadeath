// Fill out your copyright notice in the Description page of Project Settings.


#include "UDGameOverWidget.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "GameOver"

void UUDGameOverWidget::DisplayScore(int RoundsCompleted, int WavesCompleted)
{
	RoundCounter->SetText(FText::Format(LOCTEXT("RoundCount", "Rounds Completed: {0}"), RoundsCompleted));

	WaveCounter->SetText(FText::Format(LOCTEXT("WaveCount", "Waves Completed: {0}"), WavesCompleted));
}

#undef LOCTEXT_NAMESPACE
