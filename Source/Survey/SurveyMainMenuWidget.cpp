// Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyMainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void USurveyMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &USurveyMainMenuWidget::HandleStartButtonClicked);
	}
	
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &USurveyMainMenuWidget::HandleSettingsButtonClicked);
	}
	
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &USurveyMainMenuWidget::HandleExitButtonClicked);
	}
}

void USurveyMainMenuWidget::HandleStartButtonClicked()
{
	if (OnStartButtonClicked.IsBound())
	{
		OnStartButtonClicked.Broadcast();
	}
}

void USurveyMainMenuWidget::HandleSettingsButtonClicked()
{
}

void USurveyMainMenuWidget::HandleExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
