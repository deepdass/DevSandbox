// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers_DevOnly/SBaseClass_ProjectileSelect.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "PlayerComps/SCharacter.h"


// Sets default values
ASBaseClass_ProjectileSelect::ASBaseClass_ProjectileSelect()
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->bVisualizeComponent = true;
	RootComponent = DefaultSceneRoot;
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>("InteractCollision");
	SphereCollision->SetSphereRadius(16.0f);
	SphereCollision->SetupAttachment(RootComponent);

	ProjectileParticle = CreateDefaultSubobject<UNiagaraComponent>("NS_Projectile");
	ProjectileParticle->SetupAttachment(SphereCollision);
	BlackHoleParticle_IF_NO_Niagara = CreateDefaultSubobject<UParticleSystemComponent>("P_Projectile");
	BlackHoleParticle_IF_NO_Niagara->SetupAttachment(SphereCollision);
	
	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	PlateMesh->SetupAttachment(RootComponent);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));
	
	ActivateAgainTime = 10.0f;
}

void ASBaseClass_ProjectileSelect::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASBaseClass_ProjectileSelect::OnOverlapBegin);
}

void ASBaseClass_ProjectileSelect::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASCharacter* PlayerCharacter = Cast<ASCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		Deactivate(PlayerCharacter);
	}
}

void ASBaseClass_ProjectileSelect::Interact_Implementation(APawn* InstigatorPawn)
{
	ASCharacter* PlayerCharacter = Cast<ASCharacter>(InstigatorPawn);
	if (PlayerCharacter)
	{
		Deactivate(PlayerCharacter);
	}
}

void ASBaseClass_ProjectileSelect::Deactivate(ASCharacter* PlayerCharacter)
{
	GetWorldTimerManager().ClearTimer(ActiveAgainTimerHandle);
	PlayerCharacter->SetPrimaryProjectile(PrimaryProjectile);

	ProjectileParticle->SetHiddenInGame(true);
	BlackHoleParticle_IF_NO_Niagara->SetHiddenInGame(true);
	SetActorEnableCollision(false);

	GetWorldTimerManager().SetTimer(ActiveAgainTimerHandle, this, &ASBaseClass_ProjectileSelect::ActivateAgain, ActivateAgainTime, false);
}

void ASBaseClass_ProjectileSelect::ActivateAgain()
{
	ProjectileParticle->SetHiddenInGame(false);
	BlackHoleParticle_IF_NO_Niagara->SetHiddenInGame(false);
	SetActorEnableCollision(true);
}
