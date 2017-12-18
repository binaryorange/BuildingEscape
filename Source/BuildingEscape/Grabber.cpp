// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include <Engine/World.h>
#include "Runtime/Engine/Public/DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandler = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandler)
	{
		// yay
	}
	else
	{
		// boo
		UE_LOG(LogTemp, Error, TEXT("%s IS MISSING PHYSICS HANDLE"), *GetOwner()->GetName())
	}
	
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// get player's view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);
	// and log to console
	// UE_LOG(LogTemp, Warning, TEXT("Position: %s || Rotation: %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());

	// do some fancy-ass math to make a debug line git drawn, dog
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(255, 55, 90),
		false,
		0.f,
		0.f,
		2.f
	);

	// perform ray cast/line trace
	FCollisionQueryParams Trace(FName(TEXT("")), false, GetOwner());

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		Trace
	);

	// store name of object that was hit
	AActor* ObjectHit = Hit.GetActor();

	// log result to console
	if (ObjectHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("You hit %s"), (*ObjectHit->GetName()));
	}

}

