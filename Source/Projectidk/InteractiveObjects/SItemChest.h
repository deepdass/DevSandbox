// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerComps/SGameplayInterface.h"
#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECTIDK_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
	void Interact_Implementation(APawn* InstigatorPawn) override;

public:
	// Sets default values for this actor's properties
	ASItemChest();
	
protected:
	
	UPROPERTY(ReplicatedUsing = "OnRep_LidOpened") // RepNotify
	bool bLidOpened = false;
	
	UFUNCTION()
	void OnRep_LidOpened();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> LidMesh;

public:
	UPROPERTY(EditAnywhere)
	float LidOpenPitch;
};
