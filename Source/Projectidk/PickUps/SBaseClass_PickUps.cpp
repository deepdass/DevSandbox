// Fill out your copyright notice in the Description page of Project Settings.


#include "SBaseClass_PickUps.h"

#include "NiagaraComponent.h"

// Sets default values
ASBaseClass_PickUps::ASBaseClass_PickUps()
{
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetCollisionProfileName("PickUps");
	RootComponent = BaseMesh;
	
	EffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(BaseMesh);
	EffectComp->bAutoActivate = false;
	
}


void ASBaseClass_PickUps::Interact_Implementation(APawn* InstigatorPawn)
{
	EffectComp->Activate(true);
	
	BaseMesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	
	FTimerHandle DeactivateForTimerHandle;
	GetWorldTimerManager().SetTimer(DeactivateForTimerHandle, this, &ASBaseClass_PickUps::Activate, DeactiveforTime, false);
}

// Called when the game starts or when spawned
void ASBaseClass_PickUps::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ASBaseClass_PickUps::Activate()
{
	EffectComp->DeactivateImmediate();
	
	BaseMesh->SetHiddenInGame(false); 
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}
