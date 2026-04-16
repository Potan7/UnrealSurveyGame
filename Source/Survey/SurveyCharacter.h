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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	class AActor* TargetMouseActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector MouseMinBounds = FVector(-50.0f, -50.0f, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector MouseMaxBounds = FVector(50.0f, 50.0f, 0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FVector2D MonitorRes = FVector2D(1920.0f, 1080.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class USurveyMonitorWidget* MonitorWidgetInstance;

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class UWidgetInteractionComponent* WidgetInteraction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MouseClickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MouseMoveAction;
	
	FVector2D MouseInputDelta;
	
	void OnMousePressed(const FInputActionValue& Value);
	void OnMouseRelease(const FInputActionValue& Value);
	
	void OnMouseMove(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
