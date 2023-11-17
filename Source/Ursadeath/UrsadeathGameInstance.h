// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "UrsadeathGameInstance.generated.h"
 
class AUDEnemy;
class AUDArena;
class AUDPlayerCharacter;
class UDataTable;

/** Holds information about spawning a given type of enemy. Ony one entry should exist on the data table per class of enemy. Squire tier enemies do not currentlt get spawn data.*/
USTRUCT(BlueprintType)
struct FEnemySpawnData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AUDEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UTexture2D> EnemyIcon;
};

/** Holds the data shared between the EnemyWaveScheme and the actual EnemyWave.*/
USTRUCT(BlueprintType)
struct FEnemyWaveCommonData
{
	GENERATED_BODY()

	/** How many squire class enemies that spawn during a given wave. Once this is depleted, squires spawn at a slow rate.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
		int32 SquireCount;

	/** The maximum Squire class enemies that may be in play at once.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxSquires;

	/** The maximum squires that may be in play during the "Squire Slow Spawn Phase".*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxSlowSpawnSquires;

	/** The amount of time that must elapse between Squire spawns during the "Squire Slow Spawn Phase"*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SquireSlowSpawnCooldown;

	/** The maximum Knight class enemies that may be in play at once.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxKnights;
};

/** A structure meant to act as a blueprint (not the Unreal ones diploid!) for how an enemy wave should behave. The Enemy Waves themselve, and what enemy types spawn in them, are generated at runtime.*/
USTRUCT(BlueprintType)
struct FEnemyWaveScheme : public FTableRowBase
{
	GENERATED_BODY()

	/** Holds the data that will be transfered directly to the Enemy Wave this scheme will be used to create.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FEnemyWaveCommonData WaveCommonData;

	/** How many different types of Knight Tier enemies that will be in a wave.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int KnightTypes;

	/** The number of Knight tier enemies that will be in the wave.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int KnightCount;

	/** Forces one of the Knight Types to be the newest Knight tier enemy added to the spawn pool.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseNewKnight;
};

/** A structure storing the information about each wave of enemies.*/
USTRUCT(BlueprintType)
struct FEnemyWave
{
	GENERATED_BODY()

public:
	/** A map linking each Knight Type to how many of that enemy should spawn during a wave. An Knight Type should only have an entry here if greater than 0 of it should spawn.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<TSubclassOf<AUDEnemy>, int32> KnightCounts;

	/** Holds the data shared between the wave and the wave scheme that it was made from. The number of each particular type of Knight tier enemy is instead held in the Knight Counts value.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FEnemyWaveCommonData WaveData;
};

/**
 * A class meant to act as Ursadeath's "Game Manager". Useful as GameInstances persist between levels.
 */
UCLASS(Abstract)
class URSADEATH_API UUrsadeathGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	/** If true, OnLevelSetup has been bound the UWorld's begin play and shouldn't be bound again.*/
		bool bLevelSetupBound = false;

protected:
	FRandomStream RandomStream;

	/** Maps the classes of non-Squire tier enemies in play to their entry in the data table so that they are easy for other parts of the program to get.*/
	TMap<TSubclassOf<AUDEnemy>, FEnemySpawnData*> EnemyDataMap;

	/** The name used to generate the game's random seed.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameRules)
		FName GameSeedName;

	/** Holds the number of waves that each round has. The index of each round corresponds with the order they appear in game.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameRules)
		TArray<int> RoundWaveCounts;

	/** The waves set to spawn during the current round.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameRules)
		TArray<FEnemyWave> CurrentRoundWaves;

	/** The current round the player is fighting.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameRules)
		int RoundNumber = 0;

	/** The time it takes in seconds for the next wave to begin after the previous wave ends.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameRules)
		float WaveStartDelay = 3;

	/** The current set of Knights types which the game will choose to place in waves. As the game progresses from round to round, more Knight types are added to the spawn pool. Though it should start the game empty, it can be editted in blueprints to allow for debugging.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnData)
		TArray<TSubclassOf<AUDEnemy>> KnightSpawnPool;

	/** The last Knight Type to be added to the spawn pool. Used for waves generated with "UseNewKnight" set to true.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnData)
		TSubclassOf<AUDEnemy> NewKnightType;

	/** The data table of FWaveSchemes used to generate waves during gameplay. The waves should be set in chronological order, starting with the first wave.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpawnData)
		UDataTable* WaveSchemeDataTable;

	/** An array of EnemyWaveSchemes generated from the WaveSchemeDataTable when the game begins.*/
	TArray<FEnemyWaveScheme*> EnemyWaveSchemes;

	/** The current wave number in the current round that the player is fighting.*/
	int RoundWaveNumber;

	/** The current wave the player is fighting. This values does not get reset to 0 after the last wave in a round is completed (like CurrentRoundWave does) and instead continues to increment. Used to tell the game which wave to generate yet.*/
	int AbsoluteWaveNumber;

public:
	/** The maximum different classes of Non-Squire tier enemy that should ever be in a single wave.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameRules)
		int MaxNonSquireTypesPerWave = 4;

	/** The maximum waves a round may ever have.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameRules)
		int MaxWavesPerRound = 5;

	/* The current arena in play.**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<AUDArena> GameArena;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<AUDPlayerCharacter> PlayerCharacter;

	/** This should contain a data table of all of the game's non-Squire tier (Knight/Champion) enemies and any other information for spawning them.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyData)
		TObjectPtr<UDataTable> EnemySpawnDataTable;

	/** The icon used for squires in their spawn counter.*/
	UPROPERTY(EditAnywhere, Category = EnemyData)
		TObjectPtr<UTexture2D> SquireIcon;

protected:
	/** Generates the given round and populates the CurrentRoundWaves value. The value stored RoundWaveCounts[RoundIndex] determines the number of waves generated. 
	* The waves are generated using the wave schemes starting from EnemyWaveSchemes[CurrentAbsoluteWave] and ending at EnemyWaveSchemes[CurrentAbsoluteWave + RoundWaveCount] inclusive. Hence, assuming CurrentAbsoluteWave is updated properly, the waves should be generated in the order they appear on the data table, with new calls of this method continuing from where the previous left off.
	* After a wave is generated, it may be spawned with the StartRound() method.*/
	UFUNCTION(BlueprintCallable)
		void GenerateRound(int RoundIndex);

	/** Starts the given wave without announcing it to player or displaying it in their HUD. Called by Start Wave after the WaveStartDelay completes.*/
	UFUNCTION(BlueprintCallable)
		void StartWaveInstant(FEnemyWave Wave);

	/** Create an Enemy Wave from the given Wave Scheme and the instance's current Knight pool.*/
	UFUNCTION(BlueprintCallable)
		FEnemyWave GenerateEnemyWave(FEnemyWaveScheme WaveScheme);

	virtual void Init() override;

	/** Called after the player and arena have been set up. */
	void OnLevelSetup();

	/** Announces a new wave to the player and displays the enemy counts to their HUD. After the GameInstance's WaveStartDelay expires, StartWaveInstant is called and the wave's start sequence is complete.*/
	void StartWave(FEnemyWave Wave);

public:
	/** Returns the Spawn Data for a given class of enemy from the SpawnDataTable.*/
	UFUNCTION(BlueprintCallable)
		FEnemySpawnData GetSpawnDataEntry(TSubclassOf<AUDEnemy> EnemyClass);

	/** Tell the game to start spawning the CurrentRoundWaves.*/
	UFUNCTION(BlueprintCallable)
		void StartRound();

	/** Should be called whenever an enemy wave is complete. Increments CurrentRoundWave and CurrentAbsoluteWave. If the round has more waves to spawn, the next wave begins. Otherwise, the next round begins and spawning ceases until StartRound is called again.*/
	UFUNCTION(BlueprintCallable)
		void ProcessEndWave();
	
	/** Update the player's Round Screen to show the current round's waves.*/
	void UpdateRoundScreen();
};