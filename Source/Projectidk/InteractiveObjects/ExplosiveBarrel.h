// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class URadialForceComponent;
class UCapsuleComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class USActionEffect;

UCLASS()
class PROJECTIDK_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<URadialForceComponent> RadComp;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Components")
	TObjectPtr<UNiagaraComponent> EffectComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<USActionEffect> StatusEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	float StatusEffectRadius;
	
	UPROPERTY(EditDefaultsOnly)
	float ImpulseThresholdForExplosion;
	
	UPROPERTY(Replicated)
	bool bExploded = false;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerExplode();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastExplode();
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
				UPrimitiveComponent* OtherComp, FVector NormalImpulse,
				const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent)
	void Explode();
}; 
