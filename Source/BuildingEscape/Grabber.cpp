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
	FindPhysicsHandler();
	SetupInput();
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab was pressed!"))
	
	// line trace and look for actors in reach of type physics body
	auto HitResult = GetPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	// do the stuff with physics handler
	if (ActorHit)
	{
		PhysicsHandler->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			false
		);
	}


}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab was released!"))
		PhysicsHandler->ReleaseComponent();
}

void UGrabber::FindPhysicsHandler()
{
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
	// get player's view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	// do some fancy-ass math to make a debug line git drawn, dog
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

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

	return Hit;
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// get player's view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	// do some fancy-ass math to make a debug line git drawn, dog
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	// perform ray cast/line trace
	FCollisionQueryParams Trace(FName(TEXT("")), false, GetOwner());

	if(PhysicsHandler->GrabbedComponent)
	{
		PhysicsHandler->SetTargetLocation(LineTraceEnd);
	}
}

