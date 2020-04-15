#include "HUD.h"
#include "Texture.h"
#include "Actor.h"
#include "Game.h"
#include <iostream>
#include "Renderer.h"

HUD::HUD(Actor* actor) : UIComponent(actor) {

	mOwner = actor;

	mFont = new Font();
	mFont->Load("Assets/Inconsolata-Regular.ttf");

	mTimerText = mFont->RenderText("00:00.00");

	mCoinCount = mFont->RenderText("00/55");

	mCheckpointText = nullptr;

	mRadar = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Radar.png");
	mArrow = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/RadarArrow.png");
	mBlip = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Blip.png");

}

HUD::~HUD() {

	mFont->Unload();
	delete mFont;

}

void HUD::SetCheckText(std::string text) {
	mCheckpointText = mFont->RenderText(text);
	mCheckTextChanged = true;
	//mTimeSinceCheck = 0.0f;
	mOwner->GetGame()->ClearCheckpointTimer();
}

void HUD::Draw(Shader* shader) {

	DrawTexture(shader, mTimerText, Vector2(-420.0f, -325.0f));
	DrawTexture(shader, mCoinCount, Vector2(-442.0f, -295.0f));

	if (mCheckTextChanged) {
		DrawTexture(shader, mCheckpointText, Vector2::Zero);
	}

	DrawTexture(shader, mRadar, Vector2(390.0f, -265.0f));
	DrawTexture(shader, mArrow, Vector2(390.0f, -265.0f));

	for (int i = 0; i < mOwner->GetGame()->mCameras.size(); i++) {

		float dist = (mOwner->GetWorldPosition() - mOwner->GetGame()->mCameras[i]->GetWorldPosition()).Length();

		if (dist > 1500.0f) {
			continue;
		}

		Vector3 difference = (mOwner->GetGame()->mCameras[i]->GetWorldPosition() - mOwner->GetWorldPosition()) * (1 / 1500.0f) * 90.0f;
		Matrix3 rotation = Matrix3::CreateRotation(atan2(-1 * mOwner->GetWorldForward().y, mOwner->GetWorldForward().x));
		Vector2 transformed = Vector2::Transform(Vector2(difference.x, difference.y), rotation);


		Vector3 rot = mOwner->GetGame()->mCameras[i]->GetWorldForward();
		Vector2 trans = Vector2::Transform(Vector2(rot.y, rot.x), rotation);
		//rot = Vector3::Normalize(rot);
		float angle = atan2(trans.y, trans.x);

		DrawTexture(shader, mBlip, Vector2(390.0f + transformed.y, -265.0f + transformed.x), 1, angle);

	}

}

void HUD::CoinPickedUp(){

	mOwner->GetGame()->AddCoin();
	mCoinCountChanged = true;

}

void HUD::Update(float deltaTime) {

	if (mCheckTextChanged) {
		mOwner->GetGame()->AddToCheckpointTimer(deltaTime);
	}

	if (mOwner->GetGame()->GetCheckpointTime() > 5.0f) {
		mCheckTextChanged = false;
		mOwner->GetGame()->ClearCheckpointTimer();
	}

	if (mCoinCountChanged) {

		mCoinCount->Unload();
		delete mCoinCount;
		std::string coins = "";
		
		int coinCount = mOwner->GetGame()->GetCoins();

		if (coinCount < 10) {
			coins += std::to_string(0);
		}

		coins += std::to_string(coinCount);
		coins += "/55";

		mCoinCountChanged = false;
		mCoinCount = mFont->RenderText(coins);

	}

	float timeCount = mOwner->GetGame()->GetTime();

	mOwner->GetGame()->SetTime(deltaTime);
	mTimerText->Unload();
	delete mTimerText;

	timeCount += deltaTime;

	std::string timer = "";

	if (timeCount / 60 < 10) {
		timer += std::to_string(0);
	}
	else {
		timer += std::to_string((int)(timeCount / 60));
	}
	
	if (timeCount / 60 < 1) {
		timer += std::to_string(0);
	}
	else {
		timer += std::to_string((int)(timeCount / 60) % 10);
	}

	timer += ':';
	int seconds = (int)(timeCount) % 60;

	if (seconds / 10 < 1) {
		timer += std::to_string(0);
	}
	else {
		timer += std::to_string(seconds / 10);
	}

	if (seconds % 10 < 1) {
		timer += std::to_string(0);
	}
	else {
		timer += std::to_string(seconds % 10);
	}

	timer += '.';

	int milliseconds = (int)((timeCount - (float)seconds) * 100.0f) % 100;

	if (milliseconds / 10 < 1) {
		timer += std::to_string(0);
	}
	else {
		timer += std::to_string(milliseconds / 10);
	}

	if (milliseconds % 10 < 1) {
		timer += std::to_string(0);
	}
	else {
		timer += std::to_string(milliseconds % 10);
	}

	mTimerText = mFont->RenderText(timer);

}