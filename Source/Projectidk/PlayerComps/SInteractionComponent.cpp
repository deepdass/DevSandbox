// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"
#include "Blueprint/UserWidget.h"
#include "UI/SWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.DebugDrawInteraction"), false, TEXT("Enable Debug Lines for Interaction Component."), ECVF_Cheat);


// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{

	TraceDistance = 700.0f;
	TraceRadius = 30.0f;
	CollisionChannel = ECC_WorldDynamic;
	
	PrimaryComponentTick.bCanEverTick = true;

}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}

void USInteractionComponent::FindBestInteractable()
{
	bool bDebugDrawInteract = CVarDebugDrawInteraction.GetValueOnAnyThread();
	
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	APawn* MyPawn = Cast<APawn>(MyOwner);
	APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
	if (!PC) return;

	FVector CameraLoc;
	FRotator CameraRot;
	PC->GetPlayerViewPoint(CameraLoc, CameraRot);

	FVector End = CameraLoc + (CameraRot.Vector() * TraceDistance);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
	
	TArray<FHitResult> Hits;
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius); 
	
	//FHitResult Hit;
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, CameraLoc, End, ObjectQueryParams );

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, CameraLoc, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	FocusedActor = nullptr;
	
	for (FHitResult Hit : Hits) 
	{
		if (bDebugDrawInteract)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 16, LineColor, false, 2.0f);
		}
    	
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<USGameplayInterface>())
		{
			FocusedActor = HitActor;
			break;
		}
	}
	
	if (FocusedActor)
	{
		if (DefaultWidgetInst == nullptr && DefaultWidgetClass)
		{
			DefaultWidgetInst = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}
		
		DefaultWidgetInst->AttachToActor = FocusedActor;
		
		if (!DefaultWidgetInst->IsInViewport())
		{
			DefaultWidgetInst->AddToViewport();
		}
	}
	else
	{
		if (DefaultWidgetInst)
		{
			DefaultWidgetInst->RemoveFromParent();
		}
	}
	
	if (bDebugDrawInteract){
		DrawDebugLine(GetWorld(), CameraLoc, End, LineColor, false, 2.0f, 0, 2.0f);
	}
}

void USInteractionComponent::PrimaryInteract()
{
	ServerInteract();
}

void USInteractionComponent::ServerInteract_Implementation()
{
	if (FocusedActor == nullptr) return;
	
	APawn* MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(FocusedActor, MyPawn);
}