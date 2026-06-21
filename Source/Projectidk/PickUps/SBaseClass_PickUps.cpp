// Fill out your copyright notice in the Description page of Project Settings.


#include "SBaseClass_PickUps.h"

#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASBaseClass_PickUps::ASBaseClass_PickUps()
{
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetCollisionProfileName("PickUps");
	RootComponent = BaseMesh;
	
	EffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(BaseMesh);
	EffectComp->bAutoActivate = false;
	
	DeactiveforTime = 10.0f;
	
	SetReplicates(true);
}

void ASBaseClass_PickUps::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASBaseClass_PickUps, bIsActive);
}


void ASBaseClass_PickUps::Interact_Implementation(APawn* InstigatorPawn)
{
	bIsActive = false;
	OnRep_IsActive();
	
	FTimerHandle DeactivateForTimerHandle;
	GetWorldTimerManager().SetTimer(DeactivateForTimerHandle, this, &ASBaseClass_PickUps::Activate, DeactiveforTime, false);
}

void ASBaseClass_PickUps::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ASBaseClass_PickUps::Activate()
{
	bIsActive = true;
	OnRep_IsActive();
}

void ASBaseClass_PickUps::OnRep_IsActive()
{
	if (bIsActive)
	{
		EffectComp->DeactivateImmediate();
		
		BaseMesh->SetHiddenInGame(false);
		SetActorEnableCollision(true);
	}
	else
	{
		EffectComp->Activate(true);
		
		BaseMesh->SetHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}