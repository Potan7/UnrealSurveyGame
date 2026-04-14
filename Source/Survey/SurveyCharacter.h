// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	FVector InitialMouseLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MouseSpeed = 2.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FaceCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
	class USkeletalMeshComponent* BodyMesh;
	
	
	class USkeletalMeshComponent* MouseMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
