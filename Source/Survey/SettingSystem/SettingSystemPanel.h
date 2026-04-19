// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingSystemPanel.generated.h"

/**
 * 
 */
UCLASS()
class SURVEY_API USettingSystemPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetResolution(int32 Width, int32 Height, const EWindowMode::Type WindowMode);
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UComboBoxString* ResolutionComboBox;
	
	UFUNCTION()
	void OnResolutionSelected(FString SelectedItem, ESelectInfo::Type SelectionType);
	
private:
	FIntPoint ParseResolution(const FString& Resolution);
	
};
