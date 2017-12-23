// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include <Engine/World.h>
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();	

	Owner = GetOwner();

	// Get the initial door angle (closed angle)
	InitialDoorAngle = Owner->GetActorRotation();

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("Pressure Plate is not assigned on %s!"), *GetOwner()->GetName())
	}
}

void UOpenDoor::OpenDoor()
{
	Owner->SetActorRotation(FRotator(0.f, OpenAngle, 0.f));
}

void UOpenDoor::CloseDoor()
{
	Owner->SetActorRotation(FRotator(0.f, InitialDoorAngle.Yaw, 0.f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetTotalMassOfActorsOnPlate() >= MassThreshold)
	{
		OpenDoor();
		DoorLastOpen = GetWorld()->GetTimeSeconds();
	}

	if ((GetWorld()->GetTimeSeconds() - DoorLastOpen) > DoorOpenDelay)
	 {
		CloseDoor();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActors;

	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OverlappingActors); // remember OUT parameter

	for (const auto* Actor : OverlappingActors)
	{
		// add up the masses of the Actors
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}

