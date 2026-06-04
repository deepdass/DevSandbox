// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrel.h"

#include "GameFramework/Character.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComp"));
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetupAttachment(RootComponent);
	
	RadComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadForceComp"));
	RadComp->SetupAttachment(SphereComp);
	
	RadComp->SetAutoActivate(false); 

}


void AExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SphereComp->OnComponentHit.AddDynamic(this, &AExplosiveBarrel::OnHit);
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
	
}


void AExplosiveBarrel::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
							 UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
							 const FHitResult& Hit)
{
	if (!OtherActor) return;
	if (OtherActor->IsA(ACharacter::StaticClass())) return; 

	Explode();
	OtherActor->Destroy();
		
	UE_LOG(LogTemp, Warning, TEXT("otherActor: %s, at game time: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds); 
		
	FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
	
}

void AExplosiveBarrel::Explode_Implementation()
{
	RadComp->FireImpulse();
	
	UE_LOG(LogTemp, Log, TEXT("BOOM!! Haha")); 
	
}
