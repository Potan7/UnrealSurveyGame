// Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyGameMode.h"

#include "SurveyCharacter.h"
#include "SurveyMainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void ASurveyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (!SurveyMainMenuWidget)
	{
		return;
	}
	
	SurveyMainMenuWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), SurveyMainMenuWidget);
	SurveyMainMenuWidgetInstance->AddToViewport(100);
	
	if (USurveyMainMenuWidget* MainMenuWidget = Cast<USurveyMainMenuWidget>(SurveyMainMenuWidgetInstance))
	{
		MainMenuWidget->OnStartButtonClicked.AddDynamic(this, &ASurveyGameMode::OnClickStartButton);
	}
	
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(SurveyMainMenuWidgetInstance->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// InputMode.SetHideCursorDuringCapture(false);
	
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;	
		
		TArray<AActor*> FoundCameras;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MenuCamera"), FoundCameras);
		if (FoundCameras.Num() > 0)
		{
			PC->SetViewTargetWithBlend(FoundCameras[0], 0.0f);
		}
	}
}

void ASurveyGameMode::OnClickStartButton()
{
	if (SurveyMainMenuWidgetInstance)
	{
		SurveyMainMenuWidgetInstance->RemoveFromParent();
		SurveyMainMenuWidgetInstance = nullptr;
	}
	
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;	
		
		if (APawn* Pawn = PC->GetPawn())
		{
			PC->SetViewTargetWithBlend(Pawn, 4.0, VTBlend_Cubic);
			
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (OnGameStart.IsBound())
				{
					OnGameStart.Broadcast();
				}
			}, 2.0f, false);
		}
	}
	
}

