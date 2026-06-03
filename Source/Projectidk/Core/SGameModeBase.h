// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h" 
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
/**
 * 
 */
UCLASS()
class PROJECTIDK_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	ASGameModeBase();
	
	virtual void StartPlay() override;
	
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	FTimerHandle TimerHandle_SpawnBot;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	float SpawnTimerInterval;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	UEnvQuery* EQSpawnBot;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	TSubclassOf<AActor> MinionClass;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	UCurveFloat* DifficultyCurve;

	UFUNCTION()
	void SpawnBotTimerElapsed();
	
	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
};
