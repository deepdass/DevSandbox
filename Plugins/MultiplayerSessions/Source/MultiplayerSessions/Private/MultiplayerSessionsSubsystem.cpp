// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	LoginCompleteDelegate(FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnLoginComplete)),
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
}

void UMultiplayerSessionsSubsystem::Login()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("EOS"));
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Login: No EOS Online Subsystem found!"));
		MultiplayerOnLoginComplete.Broadcast(false);
		return;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (!Identity.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Login: Identity interface is invalid!"));
		MultiplayerOnLoginComplete.Broadcast(false);
		return;
	}

	// Skip login if already authenticated (e.g. editor play sessions).
	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login: Already logged in, skipping."));
		MultiplayerOnLoginComplete.Broadcast(true);
		return;
	}

	LoginCompleteDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(
		0,
		FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnLoginComplete)
	);

	FOnlineAccountCredentials Credentials;
#if WITH_EDITOR
	// Editor/PIE: use the EOS Developer Authentication Tool.
	// Setup (one-time):
	//   1. Run the Dev Auth Tool (ships with the EOS SDK).
	//   2. Sign in and create a named credential (e.g. "test1").
	//   3. Match the port and credential name below.
	Credentials.Type  = TEXT("developer");
	Credentials.Id    = TEXT("127.0.0.1:6300"); // <-- match Dev Auth Tool address
	Credentials.Token = TEXT("test1");           // <-- match credential name
#else
	// Packaged/shipping: players sign in via the Epic account portal browser flow.
	Credentials.Type = TEXT("accountportal");
#endif
	Identity->Login(0, Credentials);
}

void UMultiplayerSessionsSubsystem::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful,
	const FUniqueNetId& UserId, const FString& Error)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("EOS"));
	if (Subsystem)
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if (Identity.IsValid())
		{
			Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteDelegateHandle);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("EOS Login: %s | Error: %s"),
		bWasSuccessful ? TEXT("Success") : TEXT("Failed"), *Error);

	MultiplayerOnLoginComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!IsValidSessionInterface())
	{
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;
		DestroySession();
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());

	// This subsystem is EOS-only (LAN is handled by separate logic elsewhere),
	// so this is always an online session, never LAN.
	LastSessionSettings->bIsLANMatch = false;

	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	// Without this flag EOS silently rejects the session and the callback never fires.
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	// FIX: ULocalPlayer::GetPreferredUniqueNetId() resolves via the default
	// OnlineSubsystem (Null, per DefaultPlatformService=Null in the .ini), so it
	// returns a Null-typed net id. FOnlineSessionEOS::CreateSession() internally
	// casts the net id to FUniqueNetIdEOS and asserts/crashes on the type mismatch.
	// We must fetch the net id directly from the EOS identity interface instead.
	IOnlineSubsystem* EOSSubsystem = IOnlineSubsystem::Get(TEXT("EOS"));
	if (!EOSSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateSession: No EOS Online Subsystem found!"));
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	IOnlineIdentityPtr Identity = EOSSubsystem->GetIdentityInterface();
	TSharedPtr<const FUniqueNetId> NetId = Identity.IsValid() ? Identity->GetUniquePlayerId(0) : nullptr;
	if (!NetId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("CreateSession: UniqueNetId is invalid. Was Login() called and did it succeed?"));
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	if (!SessionInterface->CreateSession(*NetId, NAME_GameSession, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!IsValidSessionInterface())
	{
		return;
	}

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;

	// This subsystem is EOS-only (LAN is handled by separate logic elsewhere),
	// so this is always an online search, never LAN.
	LastSessionSearch->bIsLanQuery = false;

	// FIX: Removed the duplicate QuerySettings.Set call that was in the original.
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	// FIX: same as CreateSession — fetch the net id from the EOS identity interface
	// directly, not from ULocalPlayer::GetPreferredUniqueNetId(), which resolves
	// via the default (Null) subsystem and would crash FOnlineSessionEOS internals.
	IOnlineSubsystem* EOSSubsystem = IOnlineSubsystem::Get(TEXT("EOS"));
	if (!EOSSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("FindSessions: No EOS Online Subsystem found!"));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	IOnlineIdentityPtr Identity = EOSSubsystem->GetIdentityInterface();
	TSharedPtr<const FUniqueNetId> NetId = Identity.IsValid() ? Identity->GetUniquePlayerId(0) : nullptr;
	if (!NetId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("FindSessions: UniqueNetId is invalid."));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	if (!SessionInterface->FindSessions(*NetId, LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// FIX: same as CreateSession/FindSessions — use the EOS identity interface's
	// net id directly instead of ULocalPlayer::GetPreferredUniqueNetId(), which
	// resolves via the default (Null) subsystem and would crash FOnlineSessionEOS.
	IOnlineSubsystem* EOSSubsystem = IOnlineSubsystem::Get(TEXT("EOS"));
	if (!EOSSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("JoinSession: No EOS Online Subsystem found!"));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	IOnlineIdentityPtr Identity = EOSSubsystem->GetIdentityInterface();
	TSharedPtr<const FUniqueNetId> NetId = Identity.IsValid() ? Identity->GetUniquePlayerId(0) : nullptr;
	if (!NetId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("JoinSession: UniqueNetId is invalid."));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	if (!SessionInterface->JoinSession(*NetId, NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

bool UMultiplayerSessionsSubsystem::IsValidSessionInterface()
{
	if (!SessionInterface.IsValid())
	{
		// This subsystem is EOS-only — LAN is handled by separate logic elsewhere,
		// so there's no fallback to the NULL/default subsystem here.
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("EOS"));
		if (Subsystem)
		{
			SessionInterface = Subsystem->GetSessionInterface();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("IsValidSessionInterface: No EOS Online Subsystem found!"));
		}
	}

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("IsValidSessionInterface: Session Interface is invalid!"));
	}

	return SessionInterface.IsValid();
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}