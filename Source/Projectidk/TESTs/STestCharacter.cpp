// Fill out your copyright notice in the Description page of Project Settings.


#include "STestCharacter.h"

#include "PlayerComps/SAttributeComponent.h"


// Sets default values
ASTestCharacter::ASTestCharacter()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = MeshComponent;
	
	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComponent");
	
}

void ASTestCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttributeComponent->OnHealthChanged.AddDynamic(this, &ASTestCharacter::OnHealthChanged);
}

void ASTestCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	if (Delta < 0.0)
	{
		MeshComponent->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}


