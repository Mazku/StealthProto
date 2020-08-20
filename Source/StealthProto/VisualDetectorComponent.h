// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Detector.h"
#include "VisualDetectorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHPROTO_API UVisualDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	ADetector* Detector;

public:	
	// Sets default values for this component's properties
	UVisualDetectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// This ratio defines at which point the detector component raised alarm
	UPROPERTY(EditAnywhere)
	float AlarmRatio;

	// This ratio defines at which point the detector component raised alarm
	UPROPERTY(EditAnywhere)
	float VisualDistance;

	UPROPERTY(EditAnywhere)
	float VisualAngle;
};
