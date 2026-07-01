// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractiveObjects/SItemChest.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerComps/SAttributeComponent.h"

// Sets default values
ASItemChest::ASItemChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;
	
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);
	
	LidOpenPitch = 115.0f;
	
	CreditAmount = 10;
	
	SetReplicates(true);
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!IsValid(InstigatorPawn))
	{
		return;
	}
	
	if (!bLidOpened)
	{
		USAttributeComponent* AttributeComp = InstigatorPawn->FindComponentByClass<USAttributeComponent>();
		if (ensure(IsValid(AttributeComp)))
		{
			AttributeComp->ApplyCreditChange(this ,CreditAmount);
		}
	}
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();
}

void ASItemChest::OnRep_LidOpened() 
{
	float CurrentPitch = bLidOpened ? LidOpenPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrentPitch, 0.0f, 0.0f));
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASItemChest, bLidOpened);
}
