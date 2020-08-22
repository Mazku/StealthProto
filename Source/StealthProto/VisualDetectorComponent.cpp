// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualDetectorComponent.h"
#include "DrawDebugHelpers.h"
#include "StealthProtoCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UVisualDetectorComponent::UVisualDetectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_EndPhysics;

	AlarmRatio = 0.8f;
	VisualDistance = 1000.0f;
	VisualAngle = FMath::DegreesToRadians(25.0f);
}


// Called when the game starts
void UVisualDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	Detector = (ADetector*)GetOwner();

}

UVisualDetectorComponent::TraceStatus UVisualDetectorComponent::CheckPlayerVisibility(FVector start, FVector end, FVector forward, AActor* player)
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Detector);
	FHitResult OutHit;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, start, end, ECC_GameTraceChannel3, CollisionParams))
	{
		if (OutHit.Actor == player)
		{
			float distanceToPlayer = FVector::Distance(start, end);
			FVector traceForward = end - start;
			traceForward.Normalize();

			float angleToPlayer = FMath::Acos(FVector::DotProduct(forward, traceForward));
			bool playerInCone = angleToPlayer < VisualAngle && distanceToPlayer < VisualDistance;

			if (playerInCone)
			{
				if (distanceToPlayer < VisualDistance * AlarmRatio)
				{
					return TraceStatus::Detected;
				}
				return TraceStatus::CloseCall;
			}
		}
	}

	return TraceStatus::NotDetected;
}

// Called every frame
void UVisualDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector detectorLocation = Detector->GetActorLocation();
	FVector detectorForward = Detector->GetActorForwardVector();

	AStealthProtoCharacter* player = (AStealthProtoCharacter*)UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector playerLocation = player->GetActorLocation();

	// Check detection by tracing player character vertically from center and quarter up and down
	FVector quarterModifier(0, 0, player->GetDefaultHalfHeight() / 2.0f);
	TraceStatus playerDetected = FMath::Max3(
		CheckPlayerVisibility(detectorLocation, playerLocation, detectorForward, player), 
		CheckPlayerVisibility(detectorLocation, playerLocation + quarterModifier, detectorForward, player), 
		CheckPlayerVisibility(detectorLocation, playerLocation - quarterModifier, detectorForward, player));

	FColor coneColor = FColor::Green;
	if (playerDetected > TraceStatus::NotDetected)
	{
		if (playerDetected == TraceStatus::Detected)
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
