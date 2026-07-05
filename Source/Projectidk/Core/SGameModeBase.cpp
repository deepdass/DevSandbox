// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComps/SAttributeComponent.h"
#include "PlayerComps/SCharacter.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable bot spawning via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 5.0f;
	
	CreditPerKill = 5;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
    ASCharacter* Player = Cast<ASCharacter>(VictimActor);
    if (Player)
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
	if (KillerPawn)
	{
		ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>();
		if (ensure(IsValid(PlayerState)))
		{
			PlayerState->ApplyCreditChange(this, CreditPerKill);
		}
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
		UE_LOG(LogTemp, Warning, TEXT("Spawning of bot is disabled via CVarSpawnBots"));
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
	
	TArray<FVector> ResultLocations = QueryInstance->GetResultsAsLocations();
	
	if (ResultLocations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, ResultLocations[0], FRotator::ZeroRotator);
			
		DrawDebugSphere(GetWorld(), ResultLocations[0], 80.0f, 20, FColor::Blue, false, 60.0f);
	}
}




void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && AttributeComp->GetIsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void ASGameModeBase::WriteSaveGame()
{
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Load Save Game %s"), *SlotName);
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Loaded Save Game %s"), *SlotName);
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created New Save Game %s"), *SlotName);
	}
}
