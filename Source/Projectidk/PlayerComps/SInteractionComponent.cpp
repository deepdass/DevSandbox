// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"


// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
    AActor* MyOwner = GetOwner();
    if (!MyOwner) return;

    APawn* MyPawn = Cast<APawn>(MyOwner);
    APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
    if (!PC) return;

    FVector CameraLoc;
    FRotator CameraRot;
    PC->GetPlayerViewPoint(CameraLoc, CameraRot);

    FVector End = CameraLoc + (CameraRot.Vector() * 700);

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    float SphereRadius = 30.0f;
    TArray<FHitResult> Hits;
    FCollisionShape Shape;
    Shape.SetSphere(SphereRadius); 
	
	//FHitResult Hit;
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, CameraLoc, End, ObjectQueryParams );

    bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, CameraLoc, End, FQuat::Identity, ObjectQueryParams, Shape);

    FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

    for (FHitResult Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->Implements<USGameplayInterface>())
        {
            ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
            break;
        }

        DrawDebugSphere(GetWorld(), Hit.ImpactPoint, SphereRadius, 16, LineColor, false, 2.0f);
    }

    DrawDebugLine(GetWorld(), CameraLoc, End, LineColor, false, 2.0f, 0, 2.0f);
}
