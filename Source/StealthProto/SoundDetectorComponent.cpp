// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundDetectorComponent.h"
#include "StealthProtoCharacter.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
USoundDetectorComponent::USoundDetectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	AlarmRatio = 0.7f;

}


// Called when the game starts
void USoundDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	Detector = (ADetector*)GetOwner();
}


// Called every frame
void USoundDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AStealthProtoCharacter* player = nullptr;
	for (TActorIterator<AStealthProtoCharacter> It(GetWorld()); It; ++It)
	{
		player = *It;
	}
	if (player != nullptr)
	{
		float soundDistance = player->GetSoundDistance();
		if (soundDistance > 0.0f)
		{
			FVector playerLocation = player->GetActorLocation();
			FVector detectorLocation = GetOwner()->GetActorLocation();

			if (FVector::Distance(playerLocation, detectorLocation) < soundDistance * AlarmRatio)
			{
				Detector->Detected();
			}
			if (FVector::Distance(playerLocation, detectorLocation) < soundDistance)
			{
				Detector->CloseCall();
			}
		}
	}

}

