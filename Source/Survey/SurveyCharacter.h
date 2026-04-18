// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "SurveyCharacter.generated.h"

UCLASS()
class SURVEY_API ASurveyCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASurveyCharacter();
	
	UFUNCTION()
	void OnGameStarted();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	class AActor* TargetMouseActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector MouseMinBounds = FVector(-50.0f, -50.0f, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector MouseMaxBounds = FVector(50.0f, 50.0f, 0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FVector2D MonitorRes = FVector2D(1920.0f, 1080.0f);

	FVector InitialMouseLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MouseSpeed = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	AActor* TargetMonitorActor;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FaceCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
	class USkeletalMeshComponent* BodyMesh;
	
	UPROPERTY()
	class USkeletalMeshComponent* MouseMesh;
	UPROPERTY()
	class USurveyMonitorWidget* MonitorWidget;
	UPROPERTY()
	class UWidgetComponent* MonitorWidgetComponent;
	
	bool bIsSensitivityReduced = false;
	float CurrentModifier = 1.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class UWidgetInteractionComponent* WidgetInteraction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MouseClickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MouseMoveAction;
	
	FVector2D MouseInputDelta;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* MouseClickSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* MouseForceClickSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* MouseClickDeniedSound;
	
	void OnMousePressed(const FInputActionValue& Value);
	void OnMouseRelease(const FInputActionValue& Value);
	
	void OnMouseMove(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartForcedSequence(const TArray<FVector2D>& TargetUIPositions, float Speed = 16.0f);
	bool IsForcedMoving() const { return bIsForcedMoving; }
	void SetMouseInput(const bool bSet) { bEnableMouseInput = bSet; }

private:
	bool bIsGameStarted = false;
	bool bEnableMouseInput = true;
	bool bIsForcedMoving = false;
	TArray<FVector2D> ForcedTargets;
	int32 CurrentTargetIndex = 0;
	float CurrentForcedSpeed = 16.0f;
	
	float ForcedMoveTimer = 0.0f;
	bool bWaitAfterClick = false;

	void UpdateForcedMovement(float DeltaTime);
	void MoveMouseToUIPosition(FVector2D UIPos);
	void SimulateClick();
	
};
