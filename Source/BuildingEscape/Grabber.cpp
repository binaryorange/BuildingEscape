// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include <Engine/World.h>
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"


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
	FindPhysicsHandler();
	SetupInput();
}

void UGrabber::Grab()
{
	// line trace and look for actors in reach of type physics body
	auto HitResult = GetPhysicsBodyInReach();
	UPrimitiveComponent *ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	// do the stuff with physics handler
	if (!PhysicsHandler) { return; }
	if (ActorHit)
	{
		PhysicsHandler->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandler) { return; }
	PhysicsHandler->ReleaseComponent();
}

void UGrabber::FindPhysicsHandler()
{
	// assign the Phyics Handler
	PhysicsHandler = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	// if Physics Handler is not found, throw an error
	if (PhysicsHandler == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s IS MISSING PHYSICS HANDLE"), *GetOwner()->GetName())
	}
}

void UGrabber::SetupInput()
{
	// look for attached input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		// bind all input actions to InputComponent
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

const FHitResult UGrabber::GetPhysicsBodyInReach() 
{
	// perform ray cast/line trace
	FCollisionQueryParams Trace(FName(TEXT("")), false, GetOwner());

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetLineTraceStart(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		Trace
	);

	return Hit;
}

FVector UGrabber::GetLineTraceStart()
{
	// get player's view point
	FVector PlayerViewPointLocation;
	FRotator NullRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, NullRotator);

	// return the result
	return PlayerViewPointLocation;

}

FVector UGrabber::GetLineTraceEnd()
{
	// get player's view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	// return the result
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandler) { return; }
	if(PhysicsHandler->GrabbedComponent)
	{
		PhysicsHandler->SetTargetLocation(GetLineTraceEnd());
	}
}
