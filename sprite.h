#ifndef SPRITE_H
#define SPRITE_H

#include <glm/glm.hpp>
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
	std::string dir;

	virtual void draw() { ssPlayer->draw(); }
	void render_quad() { Quad::draw(); }

	void pause();
	void unpause();
	void replay();
	void next_anim();
	void previous_anim();
	void toggle_looping();

	bool is_looping() const;
	bool is_paused() const;

	float get_play_speed() const;
	void set_play_speed(float speed);

	std::string get_anim_name() const;

private:
	int anim_index = 0;
	float play_speed = 1.0f;
	bool looping = false;
	bool paused = false;
};


#endif
