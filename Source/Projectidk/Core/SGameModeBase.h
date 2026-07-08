// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h" 
#include "Engine/DataTable.h"
#include "SGameModeBase.generated.h"

class USSaveGame;
class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class UDataTable;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 1.0f;
		KillReward = 15.0;
	}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
	
};

/**
 * 
 */
UCLASS()
class PROJECTIDK_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	
	FString SlotName;
	
	UPROPERTY(EditDefaultsOnly)
	int32 CreditPerKill;
	
	UPROPERTY()
	TObjectPtr<USSaveGame> CurrentSaveGame;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	FTimerHandle TimerHandle_SpawnBot;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	float SpawnTimerInterval;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	TObjectPtr<UEnvQuery> EQSpawnBot;
	
	
	TObjectPtr<UDataTable> MonsterDataTable;
	
	//UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	//TSubclassOf<AActor> MinionClass;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	TObjectPtr<UCurveFloat> DifficultyCurve;

	UFUNCTION()
	void SpawnBotTimerElapsed();
	
	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	TMap<AController*, FTimerHandle> RespawnTimerHandles;
	
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	
public:
	
	ASGameModeBase();
	
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
	
	virtual void StartPlay() override;
	
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);
	
	UFUNCTION(Exec)
	void KillAll();
	
	UFUNCTION(BlueprintCallable, Category= "SaveGame")
	void WriteSaveGame();
	
	void LoadSaveGame();
};
