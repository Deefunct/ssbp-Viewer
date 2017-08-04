#include "sprite.h"

void Sprite::pause() {
	this->paused = true;
	ssPlayer->animePause();
}

void Sprite::unpause() {
	this->paused = false;
	ssPlayer->animeResume();
}
