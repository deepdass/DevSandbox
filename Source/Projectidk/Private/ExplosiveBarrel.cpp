// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrel.h"

#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	
	RadComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadForceComp"));
	RadComp->SetupAttachment(SphereComp);
	
	


}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->OnComponentHit.AddDynamic(this, &AExplosiveBarrel::OnHit);
	
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
	// don't overlap with yourself
	if (OtherActor && OtherActor != this)
	{
		RadComp->FireImpulse();
		OtherActor->Destroy();
		
	}
}
