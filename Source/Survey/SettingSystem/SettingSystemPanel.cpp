// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingSystemPanel.h"

#include "Components/ComboBoxString.h"
#include "GameFramework/GameUserSettings.h"

void USettingSystemPanel::SetResolution(const int32 Width, const int32 Height, const EWindowMode::Type WindowMode)
{
	if (UGameUserSettings* UserSettings = GEngine->GetGameUserSettings())
	{
		UserSettings->SetScreenResolution(FIntPoint(Width, Height));
		UserSettings->SetFullscreenMode(WindowMode);
		UserSettings->ApplySettings(false);
	}
	
}

void USettingSystemPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;
	
	if (ResolutionComboBox)
	{
		ResolutionComboBox->ClearOptions();
		ResolutionComboBox->AddOption(TEXT("1280x720"));
		ResolutionComboBox->AddOption(TEXT("1600x900"));
		ResolutionComboBox->AddOption(TEXT("1920x1080"));
		ResolutionComboBox->AddOption(TEXT("2560x1440"));

		// 저장된 해상도가 있으면 콤보박스에 기본 선택으로 설정
		const FIntPoint CurrentRes = Settings->GetScreenResolution();
		const FString ResString = FString::Printf(TEXT("%dx%d"), CurrentRes.X, CurrentRes.Y);
		
		ResolutionComboBox->SetSelectedOption(ResString);
		
		UE_LOG(LogTemp, Warning, TEXT("Resolution changed to: %s"), *ResString);
		
		ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &USettingSystemPanel::OnResolutionSelected);
		
	}
	
}

void USettingSystemPanel::OnResolutionSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Direct) return;
	
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;
	
	FIntPoint NexRes = ParseResolution(SelectedItem);
	
	Settings->SetScreenResolution(NexRes);
	Settings->ApplySettings(true);
	
	UE_LOG(LogTemp, Warning, TEXT("Resolution changed to: %s"), *SelectedItem);
}

FIntPoint USettingSystemPanel::ParseResolution(const FString& Resolution)
{
	FString Left, Right;
	if (Resolution.Split(TEXT("x"), &Left, &Right))
	{
		return FIntPoint(FCString::Atoi(*Right), FCString::Atoi(*Left));
	}
	return FIntPoint(1920, 1080);
}
