#pragma once
#include "framework.h"
#include "Offsets.h"
#include <cstdio>

namespace Hooks
{
	inline bool (*ReadyToStartMatch_OG)(AFortGameModeAthena*) = nullptr;

	bool ReadyToStartMatch(AFortGameModeAthena* thisptr) {
		static int NumberOfCalls = 0;

		if (!thisptr || !thisptr->GameState)
			return false;

		AFortGameStateAthena* GameState = (AFortGameStateAthena*)thisptr->GameState;

		if (NumberOfCalls == 0) {
			UFortPlaylistAthena* Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
			if (Playlist) {
				GameState->CurrentPlaylistData = Playlist;
				thisptr->CurrentPlaylistId = Playlist->PlaylistId;
				thisptr->CurrentPlaylistName = Playlist->PlaylistName;
				GameState->CurrentPlaylistId = Playlist->PlaylistId;
			}

			thisptr->WarmupRequiredPlayerCount = 1;

			if (thisptr->AlivePlayers.Num() >= thisptr->WarmupRequiredPlayerCount) {
				GameState->OnRep_CurrentPlaylistData();
				GameState->OnRep_CurrentPlaylistId();
				return true;
			}

			NumberOfCalls++;
		}

		if (NumberOfCalls == 1) {
			UWorld* World = UWorld::GetWorld();
			if (World) {
				auto GameNetDriverName = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
				UNetDriver* NetDriver = Sarah::Funcs::CreateNetDriver(UEngine::GetEngine(), World, GameNetDriverName);
				if (NetDriver) {
					NetDriver->World = World;
					World->NetDriver = NetDriver;
					NetDriver->NetDriverName = GameNetDriverName;

					FURL URL{};
					URL.Port = 7777;

					FString TemporaryString;
					Sarah::Funcs::InitListen(NetDriver, World, URL, false, TemporaryString);
					Sarah::Funcs::SetWorld(NetDriver, World);

					for (auto& LevelCollection : World->LevelCollections) {
						LevelCollection.NetDriver = NetDriver;
					}

					thisptr->bWorldIsReady = true;
					SetConsoleTitleA("Listening is now on - 3.6GENOS GS");
					NumberOfCalls++;
				}
			}
		}

		return thisptr->AlivePlayers.Num() >= thisptr->WarmupRequiredPlayerCount;
	}

	APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* Controller, AActor* StartSpot) {
		APawn* Pawn = nullptr;
		if (GameMode && Controller && StartSpot) {
			Pawn = GameMode->SpawnDefaultPawnAtTransform(Controller, StartSpot->GetTransform());
		}
		if (Pawn && Controller) {
			auto PlayerState = (AFortPlayerStateAthena*)Controller->PlayerState;
			if (PlayerState) {
				auto Loadout = Controller->CustomizationLoadout;
				if (Loadout.Character && Loadout.Character->HeroDefinition) {
					auto FortPawn = (APlayerPawn_Athena_C*)Pawn;
					if (FortPawn) {
						FortPawn->CustomizationLoadout = Loadout;
						FortPawn->OnRep_CustomizationLoadout();
						PlayerState->HeroType = Loadout.Character->HeroDefinition;
						PlayerState->OnRep_HeroType();
						auto Apply = (void(*)(void*, void*))(ImageBase + 0xf32360);
						Apply(PlayerState, FortPawn);
					}
				}
			}
		}
		return Pawn;
	}

	inline void (*TickFlush_OG)(UNetDriver* NetDriver, float DeltaSeconds);
	void TickFlush(UNetDriver* Driver, float DeltaSeconds)
	{
		if (Driver && Driver->ReplicationDriver) {
			Sarah::Funcs::ServerReplicateActors(Driver->ReplicationDriver, DeltaSeconds);
		}

		return TickFlush_OG(Driver, DeltaSeconds);
	}
}

namespace Patches
{
	int ReturnTrue() {
		return 1;
	}

	void ReturnHook() {
		return;
	}
}

inline void* (*DispatchRequest_OG)(void*, void*, int) = nullptr;
void* DispatchRequestHook(void* Arg1, void* MCPData, int Arg3) {
	if (MCPData)
		*reinterpret_cast<int*>(reinterpret_cast<intptr_t>(MCPData) + 0x60) = 3;
	if (DispatchRequest_OG)
		return DispatchRequest_OG(Arg1, MCPData, 3);
	return nullptr;
}