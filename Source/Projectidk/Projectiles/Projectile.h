// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseClassProjectile.h"
#include "projectile.generated.h"


UCLASS()
class PROJECTIDK_API Aprojectile : public ASBaseClassProjectile
{
	GENERATED_BODY()

public:
	Aprojectile();

protected:
	
	virtual void BeginPlay() override;
	
	
	virtual void Explode_Implementation() override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UNiagaraSystem* SpawnVFX;
	
};