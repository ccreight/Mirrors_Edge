#pragma once
#include "UIComponent.h"
#include "Actor.h"
#include "Font.h"

class HUD : public UIComponent{

public:
	HUD(Actor* actor);
	~HUD();
	void Draw(Shader* shader);
	void Update(float deltaTime);
	void CoinPickedUp();
	void SetCheckText(std::string text);

private:
	Actor* mOwner;
	Font* mFont;
	class Texture* mTimerText;
	//float mTime;

	//int mCoins = 0;
	bool mCoinCountChanged = true;
	class Texture* mCoinCount;

	class Texture* mCheckpointText;
	bool mCheckTextChanged = false;
	//float mTimeSinceCheck = 0.0f;

	class Texture* mRadar;
	class Texture* mArrow;
	class Texture* mBlip;

};