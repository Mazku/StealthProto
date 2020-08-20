// Fill out your copyright notice in the Description page of Project Settings.


#include "Detector.h"
#include "Kismet/GameplayStatics.h"
#include "StealthProtoCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADetector::ADetector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachTo(Root);

}

// Called when the game starts or when spawned
void ADetector::BeginPlay()
{
	Super::BeginPlay();
	
	Active = true;
	float activeDurationInSeconds = (float)ActiveDuration.GetTotalSeconds();
	// Offset active started to have happened between 0 to activeDuration * 0.5. So not every detector have synchronized activations
	ToggleStarted = FDateTime::Now() + FTimespan::FromSeconds(FMath::RandRange(0.0f, activeDurationInSeconds * 0.5f));
}

// Called every frame
void ADetector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FDateTime now = FDateTime::Now();
	if (Active && now > ToggleStarted + ActiveDuration)
	{
		Active = false;
		ToggleStarted = now;
	}
	else if (now > ToggleStarted + PassiveDuration)
	{
		Active = true;
		ToggleStarted = now;
	}
	if (now > ModeStarted + ResetDuration)
	{
		if (Alarmed)
		{
			Alarmed = false;
		}
		else if (Concerned)
		{
			Concerned = false;
		}
		ModeStarted = now;
	}
	UpdateDetectorState();

	if (Active && Alarmed)
	{
		AStealthProtoCharacter * player = (AStealthProtoCharacter*)UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		player->DetectedThisTick();
	}
}

void ADetector::UpdateDetectorState()
{
	UMaterial* materialToUse = ActiveMaterial;
	if (!Active)
	{
		materialToUse = PassiveMaterial;
	}
	else if (Alarmed)
	{
		materialToUse = AlarmedMaterial;
	}
	else if (Concerned)
	{
		materialToUse = ConcernedMaterial;
	}
	Mesh->SetMaterial(0, materialToUse);
}

void ADetector::Detected()
{
	if (Active)
	{
		ModeStarted = FDateTime::Now();
		ToggleStarted = FDateTime::Now();
		Alarmed = true;
		UpdateDetectorState();
	}
}

void ADetector::CloseCall()
{
	if (Active)
	{
		ModeStarted = FDateTime::Now();
		ToggleStarted = FDateTime::Now();
		Concerned = true;
		UpdateDetectorState();
	}
}
