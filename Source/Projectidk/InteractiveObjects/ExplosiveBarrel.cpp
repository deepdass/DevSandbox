#include "ExplosiveBarrel.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);

	SphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComp"));
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetupAttachment(RootComponent);
	
	RadComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadForceComp"));
	RadComp->SetupAttachment(SphereComp);
	RadComp->SetAutoActivate(false);
}

void AExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AExplosiveBarrel, bExploded);
}

void AExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComp->OnComponentHit.AddDynamic(this, &AExplosiveBarrel::OnHit);
}

void AExplosiveBarrel::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
							 UPrimitiveComponent* OtherComp, FVector NormalImpulse,
							 const FHitResult& Hit)
{
	if (!OtherActor || bExploded) return;
	if (OtherActor->IsA(ACharacter::StaticClass())) return;

	if (HasAuthority())
		ServerExplode(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("otherActor: %s, at game time: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

	FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
}

void AExplosiveBarrel::ServerExplode_Implementation(AActor* OtherActor)
{
	if (bExploded) return;
	bExploded = true;
	if (OtherActor) OtherActor->Destroy();
	MulticastExplode();
}

void AExplosiveBarrel::MulticastExplode_Implementation()
{
	Explode();
}

void AExplosiveBarrel::Explode_Implementation()
{
	RadComp->FireImpulse();
	bExploded = false;
	UE_LOG(LogTemp, Log, TEXT("BOOM!! Haha"));
}