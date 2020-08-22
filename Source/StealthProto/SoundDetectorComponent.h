#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Detector.h"
#include "SoundDetectorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHPROTO_API USoundDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	ADetector* Detector;


public:	
	// Sets default values for this component's properties
	USoundDetectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// This ratio defines at which point the detector component raised alarm
	UPROPERTY(EditAnywhere)
	float AlarmRatio;
};
