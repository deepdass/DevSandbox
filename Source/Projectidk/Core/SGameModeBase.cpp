// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "AI/SMonsterData.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerComps/SAttributeComponent.h"
#include "PlayerComps/SCharacter.h"
#include "SaveSystem/SSaveGameSubsystem.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable bot spawning via timer."), ECVF_Cheat);


static FMonsterInfoRow* GetRandomWeightedMonsterRow(const TArray<FMonsterInfoRow*>& Rows)
{
	float TotalWeight = 0.0f;
	for (FMonsterInfoRow* Row : Rows)
	{
		if (Row)
		{
			TotalWeight += FMath::Max(Row->Weight, 0.0f);
		}
	}

	if (TotalWeight <= 0.0f)
	{
		return Rows.IsValidIndex(0) ? Rows[FMath::RandRange(0, Rows.Num() - 1)] : nullptr;
	}

	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);

	for (FMonsterInfoRow* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}

		const float RowWeight = FMath::Max(Row->Weight, 0.0f);
		if (RandomWeight <= RowWeight)
		{
			return Row;
		}

		RandomWeight -= RowWeight;
	}
	
	return Rows.Last();
}


static float GetKillRewardForMonster(const UDataTable* MonsterDataTable, const FPrimaryAssetId& MonsterId)
{
	if (!MonsterDataTable || !MonsterId.IsValid())
	{
		return 0.0f;
	}

	TArray<FMonsterInfoRow*> Rows;
	MonsterDataTable->GetAllRows("", Rows);

	for (const FMonsterInfoRow* Row : Rows)
	{
		if (Row && Row->MonsterId == MonsterId)
		{
			return Row->KillReward;
		}
	}

	return 0.0f;
}


ASGameModeBase::ASGameModeBase()
{
	PlayerStateClass = ASPlayerState::StaticClass();
	
	SpawnTimerInterval = 5.0f;
	
	DefaultCreditPerKill = 15;
	RagePerKill = 20;
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	USSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	
	SaveGameSubsystem->LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
    ASCharacter* Player = Cast<ASCharacter>(VictimActor);
    if (IsValid(Player))
    {
        AController* Controller = Player->GetController();
        if (!Controller) { return; }

        if (FTimerHandle* ExistingHandle = RespawnTimerHandles.Find(Controller))
        {
            if (GetWorldTimerManager().IsTimerActive(*ExistingHandle))
            {
                return;
            }
        }
        FTimerHandle& TimerHandle = RespawnTimerHandles.FindOrAdd(Controller);

        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUFunction(this, "RespawnPlayerElapsed", Controller);

        float SpawnDelay = 2.5f;
        GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, SpawnDelay, false);
    }
	
	APawn* KillerPawn = Cast<APawn>(Killer);
	
	if (IsValid(KillerPawn))
	{
		ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>();
		if (ensure(IsValid(PlayerState)))
		{
			float CreditReward = DefaultCreditPerKill;
			
			ASAICharacter* KilledMonster = Cast<ASAICharacter>(VictimActor);
			if (IsValid(KilledMonster) && KilledMonster->GetMonsterData())
			{
				CreditReward = GetKillRewardForMonster(MonsterDataTable, KilledMonster->GetMonsterData()->GetPrimaryAssetId());
			}
			
			PlayerState->ApplyCreditChange(this, CreditReward);
			UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PickupEnvQuery, KillerPawn, EEnvQueryRunMode::AllMatching, nullptr);
			if (IsValid(QueryInstance))
			{
				QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPickupSpawnQueryCompleted);
			}
		}
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(KillerPawn);
		if (IsValid(AttributeComp))
		{
			AttributeComp->ApplyRageChange(KillerPawn, RagePerKill);
		}
	}
}

void ASGameModeBase::OnPickupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	
	if (Locations.Num() > 0)
	{
		int32 RandomLocIndex = FMath::RandRange(0, Locations.Num() - 1);
		FVector Location = Locations[RandomLocIndex];
		Location.Z += 50.0f;
			
		int32 RandomPickupIndex = FMath::RandRange(0, PickupArray.Num() - 1);
		TSubclassOf<AActor> PickupClass = PickupArray[RandomPickupIndex];
			
		GetWorld()->SpawnActor<AActor>(PickupClass, Location, FRotator::ZeroRotator);
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
    if (IsValid(Controller))
    {
        RespawnTimerHandles.Remove(Controller);

        Controller->UnPossess();
        RestartPlayer(Controller);
    }
}


void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning of bot is disabled via CVarSpawnBots command"));
		return;
	}
	
	int32 NoOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && AttributeComp->GetIsAlive())
		{
			NoOfAliveBots++;
		}
	}
	
	float MaxBotCount = 3;
	
	if (ensure(DifficultyCurve))
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	if (NoOfAliveBots >= MaxBotCount){ return;}
		
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, EQSpawnBot, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	
	if (ensure(IsValid(QueryInstance)))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryFinished);
	}
	
}

void ASGameModeBase::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("EQS Query failed with status: %d"), (int32)QueryStatus);
		return;
	}
	
	const TArray<FVector> ResultLocations = QueryInstance->GetResultsAsLocations();
	if (ResultLocations.IsValidIndex(0))
	{
		if (MonsterDataTable)
		{
			
			TArray<FMonsterInfoRow*> Rows;
			MonsterDataTable->GetAllRows("", Rows);
			
			FMonsterInfoRow* SelectedRow = GetRandomWeightedMonsterRow(Rows);
			if (!SelectedRow)
			{
				return;
			}
			
			UAssetManager* Manager = UAssetManager::GetIfInitialized();
			if (Manager)
			{
				UE_LOG(LogTemp, Log, TEXT("Monster Loading.."));
				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, ResultLocations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedID, FVector SpawnLocation)
{
	UE_LOG(LogTemp, Log, TEXT("Monster Loaded"));
	UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedID));
		if (MonsterData)
		{
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			
			ASAICharacter* SpawnedMonster = Cast<ASAICharacter>(SpawnedActor);
			if (IsValid(SpawnedMonster))
			{
				SpawnedMonster->SetMonsterData(MonsterData);
			}
		}
	}
	DrawDebugSphere(GetWorld(), SpawnLocation, 80.0f, 20, FColor::Blue, false, 60.0f);
}