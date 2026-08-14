// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMonsterData.generated.h"

class USAction;
/**
 * 
 */
UCLASS()
class PROJECTIDK_API USMonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override{
		return FPrimaryAssetId("Monsters", GetFName());
	};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TSubclassOf<AActor> MonsterClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TArray<TSubclassOf<USAction>> Actions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTexture2D> Icon;
};