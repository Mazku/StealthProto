// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualDetectorComponent.h"
#include "DrawDebugHelpers.h"
#include "SceneView.h"
#include "StealthProtoCharacter.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UVisualDetectorComponent::UVisualDetectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	AlarmRatio = 0.7f;
	VisualDistance = 1000.0f;
	VisualAngle = FMath::DegreesToRadians(25.0f);
}


// Called when the game starts
void UVisualDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	Detector = (ADetector*)GetOwner();

}

// Called every frame
void UVisualDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector detectorLocation = Detector->GetActorLocation();
	FVector detectorForward = Detector->GetActorForwardVector();

	AStealthProtoCharacter* player = nullptr;
	for (TActorIterator<AStealthProtoCharacter> It(GetWorld()); It; ++It)
	{
		player = *It;
	}
	FVector playerLocation = player->GetActorLocation();

	FVector traceForward = playerLocation - detectorLocation;
	traceForward.Normalize();
	float angleToPlayer = FMath::Acos(FVector::DotProduct(detectorForward, traceForward));
	float distanceToPlayer = FVector::Distance(detectorLocation, playerLocation);

	FVector Start = detectorLocation;
	FVector End = playerLocation;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Detector);
	FHitResult OutHit;

	bool playerHit = false;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_GameTraceChannel3, CollisionParams))
	{
		if (OutHit.Actor == player)
		{
			playerHit = true;
		}
	}
	bool playerInCone = angleToPlayer < VisualAngle && distanceToPlayer < VisualDistance;

	bool playerDetected = false;
	if (playerHit && playerInCone)
	{
		playerDetected = true;
	}

	FColor coneColor = FColor::Green;
	if (playerDetected)
	{
		if (distanceToPlayer < VisualDistance * AlarmRatio)
		{
			coneColor = FColor::Red;
			Detector->Detected();
		}
		else 
		{
			coneColor = FColor::Yellow;
			Detector->CloseCall();
		}
	}
	DrawDebugCone(GetWorld(), detectorLocation, detectorForward, VisualDistance, VisualAngle, VisualAngle, 30, coneColor);
}
