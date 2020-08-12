// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Detector.generated.h"

UCLASS()
class STEALTHPROTO_API ADetector : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	bool Alarmed;

	UPROPERTY()
	bool Concerned;

	UPROPERTY()
	bool Active;

	UPROPERTY()
	FDateTime ModeStarted;

	UPROPERTY()
	FDateTime ToggleStarted;

	void UpdateDetectorState();

public:	
	// Sets default values for this actor's properties
	ADetector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CloseCall();

	void Detected();

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UMaterial* AlarmedMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* ConcernedMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* ActiveMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* PassiveMaterial;

	UPROPERTY(EditAnywhere)
	FTimespan ActiveDuration;

	UPROPERTY(EditAnywhere)
	FTimespan PassiveDuration;

	UPROPERTY(EditAnywhere)
	FTimespan ResetDuration;

};
