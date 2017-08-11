#include "sprite.h"

void Sprite::pause() {
	paused = true;
	ssPlayer->animePause();
}

void Sprite::unpause() {
	paused = false;
	ssPlayer->animeResume();
}

void Sprite::replay()
{
	ssPlayer->play(animation_list[anim_index], 1);
	ssPlayer->setStep(play_speed);
}

void Sprite::next_anim()
{
	anim_index = (anim_index == 0) ? animation_list.size() - 1 : --anim_index;
	ssPlayer->play(animation_list[anim_index], !looping);
	ssPlayer->setStep(play_speed);
	unpause();
}

void Sprite::previous_anim()
{
	anim_index = (anim_index == animation_list.size() - 1) ? 0 : ++anim_index;
	ssPlayer->play(animation_list[anim_index], !looping);
	ssPlayer->setStep(play_speed);
	unpause();
}

bool Sprite::is_looping() const
{
	return looping;
}

bool Sprite::is_paused() const
{
	return paused;
}

float Sprite::get_play_speed() const
{
	return play_speed;
}

void Sprite::set_play_speed(float speed)
{
	play_speed = speed;
	ssPlayer->setStep(play_speed);
}

std::string Sprite::get_anim_name() const
{
	return ssPlayer->getPlayAnimeName();
}

void Sprite::toggle_looping()
{
	ssPlayer->play(animation_list[anim_index], looping, ssPlayer->getFrameNo());
	ssPlayer->setStep(play_speed);
	looping = !looping;
	unpause();
}
