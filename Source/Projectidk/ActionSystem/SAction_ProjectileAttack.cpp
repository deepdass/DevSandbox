// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"

#include "Projectiles/SBaseClassProjectile.h"
#include "GameFramework/Character.h"



USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	AttackAnimDelay = 0.2f;
}

void USAction_ProjectileAttack::SetPrimaryProjectile(TSubclassOf<ASBaseClassProjectile> NewProjectile)
{
	ProjectileClass = NewProjectile;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	
	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);
		
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		APlayerController* PC = Cast<APlayerController>(InstigatorCharacter->GetController());
        if (!PC) return;
		
		FVector CamStart;
		FRotator CamRot; 
		PC->GetPlayerViewPoint(CamStart, CamRot);

		const FVector TraceEnd  = CamStart + (CamRot.Vector() * 5000.0f);
	
		FHitResult HitResult;
		
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CamStart, TraceEnd , ECC_Visibility, Params); 

		FVector HandLoc = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName); 
		FRotator ProjectileRot = CamRot;
		if (bHit)
		{
			FVector ImpactPt = HitResult.ImpactPoint;
			ProjectileRot = (ImpactPt - HandLoc).GetSafeNormal().Rotation();
		}

		FTransform SpawnTM = FTransform(ProjectileRot, HandLoc);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		GetWorld()->SpawnActor<ASBaseClassProjectile>(ProjectileClass, SpawnTM, SpawnParams);
	}
	
	StopAction(InstigatorCharacter);
}



