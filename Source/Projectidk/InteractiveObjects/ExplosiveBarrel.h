// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class URadialForceComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class PROJECTIDK_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:
	UPROPERTY(BlueprintReadOnly) 
	TObjectPtr<UStaticMeshComponent> SphereComp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URadialForceComponent> RadComp;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Components")
	TObjectPtr<UNiagaraComponent> EffectComp;
	
	
	UPROPERTY(Replicated)
	bool bExploded = false;

	UFUNCTION(Server, Reliable)
	void ServerExplode(AActor* OtherActor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastExplode();
	
	virtual void PostInitializeComponents() override; 
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
				UPrimitiveComponent* OtherComp, FVector NormalImpulse,
				const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) 
	void Explode();
}; 
