#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "Player.h"
#include "Checkpoint.h"
#include "SecurityCamera.h"

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddBlock(class Actor* actor);
	void RemoveBlock(class Actor* actor);

	Mix_Chunk* GetSound(const std::string& fileName);

	class Renderer* GetRenderer() { return mRenderer; }

	Player* GetPlayer() {
		return mPlayer;
	}

	int GetChannel() {
		return mChannel;
	}

	Player* mPlayer = nullptr;
	std::vector<class Actor*> mBlocks;
	std::vector<Checkpoint*> mCheckpoints;
	std::vector<SecurityCamera*> mCameras;

	int mCurrentCheckpoint = 0;

	void SetNextLevel(std::string lvl) {
		mNextLevel = lvl;
	}

	float GetTime() {
		return mTimeCount;
	}

	int GetCoins() {
		return mCoinCount;
	}

	void SetTime(float f) {
		mTimeCount += f;
	}

	void AddCoin() {
		mCoinCount++;
	}

	float GetCheckpointTime() {
		return mCheckpointTimer;
	}

	void AddToCheckpointTimer(float f) {
		mCheckpointTimer += f;
	}

	void ClearCheckpointTimer() {
		mCheckpointTimer = 0.0f;
	}

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	const float WINDOW_WIDTH = 1024.0f, WINDOW_HEIGHT = 768.0f;

	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// All the actors in the game
	std::vector<class Actor*> mActors;

	class Renderer* mRenderer = nullptr;

	Uint32 mTicksCount = 0;
	bool mIsRunning;

	int mChannel = 0;
	std::string mNextLevel;

	void LoadNextLevel();

	float mTimeCount = 0.0f;
	int mCoinCount = 0;
	float mCheckpointTimer = 0.0f;

};
