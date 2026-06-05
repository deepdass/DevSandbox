// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerComps/SGameplayInterface.h"
#include "SBaseClass_ProjectileSelect.generated.h"

class ASCharacter;
class UNiagaraComponent;
class USphereComponent;
class UBoxComponent;
class UStaticMeshComponent;
class ASBaseClassProjectile;

UCLASS()
class PROJECTIDK_API ASBaseClass_ProjectileSelect : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASBaseClass_ProjectileSelect();
	
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
	void PostInitializeComponents() override;

protected:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> ProjectileParticle;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlateMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxCollision;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	FTimerHandle ActiveAgainTimerHandle;
	UPROPERTY(EditDefaultsOnly)
	float ActivateAgainTime;
	
	void Deactivate(ASCharacter* PlayerCharacter);
	
	void ActivateAgain();
	
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ASBaseClassProjectile> PrimaryProjectile;
};
