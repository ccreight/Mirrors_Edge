//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Random.h"
#include "LevelLoader.h"
#include <SDL/SDL_ttf.h>

Game::Game()
:mIsRunning(true)
{
	
}

bool Game::Initialize()
{
	Random::Init();

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// TODO: Create renderer
	mRenderer = new Renderer(this);
	bool result = mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

	if (!result) {
		return result;
	}

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_AllocateChannels(32);
	Mix_GroupChannels(22, 31, 1);

	TTF_Init();

	LoadData();

	mTicksCount = SDL_GetTicks();

	SDL_SetRelativeMouseMode(SDL_TRUE); // Enable relative mouse mode
	SDL_GetRelativeMouseState(nullptr, nullptr); // Clear any saved values
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();

		if (!mNextLevel.empty()) {
			LoadNextLevel();
		}

	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.01667f)
	{
		deltaTime = 0.01667f;
	}
	mTicksCount = SDL_GetTicks();
	
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}

}

void Game::GenerateOutput()
{
	// TODO: tell renderer to draw
	mRenderer->Draw();
}

void Game::LoadData()
{

	//mChannel = Mix_PlayChannel(-1, GetSound("Assets/Sounds/ShipLoop.wav"), -1);

	Matrix4 projectionMatrix = Matrix4::CreatePerspectiveFOV(1.22f, WINDOW_WIDTH, WINDOW_HEIGHT, 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(projectionMatrix);

	Matrix4 viewMatrix = Matrix4::CreateLookAt(Vector3(-300, 0, 100), Vector3(20, 0, 0), Vector3::UnitZ);
	mRenderer->SetViewMatrix(viewMatrix);

	Mix_Chunk* sound = GetSound("Assets/Sounds/Music.ogg");
	mChannel = Mix_PlayChannel(-1, sound, -1);
	/*GetSound("Assets/Sounds/Fall.wav");
	GetSound("Assets/Sounds/FinalLap.wav");
	GetSound("Assets/Sounds/Lost.wav");
	GetSound("Assets/Sounds/Music.ogg");
	GetSound("Assets/Sounds/MusicFast.ogg");
	GetSound("Assets/Sounds/RaceStart.wav");
	GetSound("Assets/Sounds/Won.wav");

	Mix_PlayChannel(-1, GetSound("Assets/Sounds/RaceStart.wav"), 0);*/

	LevelLoader::Load(this, "Assets/Tutorial.json");
	
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
	mRenderer->Shutdown();
	delete mRenderer;
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}

void Game::AddBlock(Actor* actor) {
	mBlocks.emplace_back(actor);
}

void Game::RemoveBlock(Actor* actor)
{
	auto iter = std::find(mBlocks.begin(), mBlocks.end(), actor);
	if (iter != mBlocks.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mBlocks.end() - 1;
		std::iter_swap(iter, iter2);
		mBlocks.pop_back();
	}
}

void Game::LoadNextLevel() {

	UnloadData();
	mCheckpoints.clear();

	LevelLoader::Load(this, mNextLevel);

	mNextLevel.clear();
	mCurrentCheckpoint = 0;
	mCheckpoints[0]->GetComponent<MeshComponent>()->SetTextureIndex(0);

}