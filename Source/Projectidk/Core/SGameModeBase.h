// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h" 
#include "Engine/DataTable.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class UDataTable;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 1.0f;
		KillReward = 15.0f;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FPrimaryAssetId MonsterId;

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
	
	// Fallback credit reward used when the victim isn't an AI monster with its own KillReward (e.g. PvP kills).
	UPROPERTY(EditDefaultsOnly)
	float DefaultCreditPerKill;
	
	UPROPERTY(EditDefaultsOnly)
	int32 RagePerKill;
	
	FTimerHandle TimerHandle_SpawnBot;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	float SpawnTimerInterval;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	TObjectPtr<UEnvQuery> EQSpawnBot;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	TObjectPtr<UDataTable> MonsterDataTable;
	
	UPROPERTY(EditDefaultsOnly, Category="SpawnBot")
	TObjectPtr<UCurveFloat> DifficultyCurve;

	UFUNCTION()
	void SpawnBotTimerElapsed();
	
	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	UPROPERTY()
	TMap<AController*, FTimerHandle> RespawnTimerHandles;
	
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	void OnMonsterLoaded(FPrimaryAssetId LoadedID, FVector SpawnLocation);
	
	UPROPERTY(EditDefaultsOnly)
	UEnvQuery* PickupEnvQuery;
	
	UFUNCTION()
	void OnPickupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> PickupArray;

	
public:
	
	ASGameModeBase();
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void StartPlay() override;
	
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

};