#ifndef SPRITE_H
#define SPRITE_H

#include "texture.h"
#include "ssbp/SS5Player.h"
#include "quad.h"
class Sprite : public Quad {
public:
	Sprite() { textures.resize(8); }
	~Sprite() {
		delete this->ssPlayer;
		delete this->resman;
	}

	ss::Player* ssPlayer; // used for sprite studio binary file playback
	ss::ResourceManager* resman; // ^
	std::vector<Texture*> textures;
	std::vector<std::string> animation_list;
	std::string file_name;
	std::string location;

	bool paused = false;

	virtual void draw() { ssPlayer->draw(); }
	void render_quad() { Quad::draw(); }
	void pause();
	void unpause();
};


#endif
