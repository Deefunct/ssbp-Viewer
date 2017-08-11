#include <iostream>
#include <iomanip>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "texture.h"
#include "quad.h"
#include "sprite.h"


Sprite sprite;
Quad background; // the background quad/plane; has its own shader

const float fivTwel = 1.0f / 256.0f; // for viewport scale
glm::vec3 mover(0.0f, -0.5f, 0.0f); // camera position
glm::vec3 scale(fivTwel, fivTwel, 1.0f); // camera view scale

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifier);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void drop_callback(GLFWwindow* window, int count, const char** paths);
void handleEvents(GLFWwindow* window);
void draw(GLFWwindow* window);

// Globals
const unsigned int WIN_WIDTH = 512;
const unsigned int WIN_HEIGHT = 512;
float aspect_ratio = WIN_WIDTH / WIN_HEIGHT;
float frame_rate = 60.0f;
bool wireframe = false; // for wireframe toggle. true when wireframe mode is on

std::string help = // hotkeys
"\nA: previous animation\n"
"S: next animation\n"
"L: toggle animation loop\n"
"Space: pause / replay(when looping is disabled)\n"
"X: flip\n"
"C: center camera\n"
"Q: screenshot\n"
"W: toggle wireframe\n"
"1, 2, 3: change animation speed\n"
"H: display this hotkey list again\n";


int main(int argc, char* argv[]) {
	// glfw: initialize and configure
	sprite.dir = argv[0];
	sprite.dir = sprite.dir.substr(0, sprite.dir.find_last_of("/\\") + 1);

	std::vector<std::string> shader_name_list{
		"shaders/sprite.vertex",
		"shaders/sprite.fragment",
		"shaders/background.vertex",
		"shaders/background.fragment"
	};
	//*
	if (argc == 1) {
		std::cout << "Drag an ssbp file here then press enter.\n";
		std::cin >> sprite.file_name;
	}
	else if (argc == 2) {
		sprite.file_name = argv[1];

		for (int i = 0; i < shader_name_list.size(); ++i) {
			shader_name_list[i] = sprite.dir + shader_name_list[i];
		}

	}
	else return 0;
	//*/
	//sprite.file_name = "images/ch02_00_Roy_M_Normal.ssbp"; // for debugging
	//sprite.file_name = "images/Eliwood/ch03_06_Eliwod_M_Normal.ssbp";
	//sprite.file_name = "images/Palla/ch01_18_Paora_F_Normal.ssbp";
	//sprite.file_name = "images/Sheeda/ch01_17_Sheeda_F_Normal.ssbp";

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "SSBP Viewer", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetDropCallback(window, drop_callback);
	glfwSetWindowSizeLimits(window, WIN_WIDTH, WIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSwapInterval(frame_rate / 60); //test //0 = unlocked frame rate; 1=60fps; 2=30fps; 3=20fps

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// initialize shaders & geometry
	sprite.init(shader_name_list[0], shader_name_list[1]);
	background.init(shader_name_list[2], shader_name_list[3]);
	background.texture = new Texture((sprite.dir + "shaders/background.png").c_str(), false); // load background image

	//glEnable(GL_CULL_FACE); //test

	sprite.resman = ss::ResourceManager::getInstance();
	sprite.ssPlayer = ss::Player::create();
	
	sprite.file_name = sprite.resman->addData(sprite.file_name);
	sprite.ssPlayer->setData(sprite.file_name, &sprite.animation_list);
	sprite.ssPlayer->play(sprite.animation_list[0], 1);
	sprite.ssPlayer->setGameFPS(frame_rate);

	std::cout << help << '\n' << sprite.file_name << "\nNumber of animations: " << sprite.animation_list.size() << "\n\n" << sprite.ssPlayer->getPlayAnimeName() << std::endl;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1); // test
	//GLuint PBO;
	//glGenBuffers(1, &PBO);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO);
															// render loop
	while (!glfwWindowShouldClose(window)) {
		draw(window);
		handleEvents(window);
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

void draw(GLFWwindow* window) {
	// calc delta time
	static GLfloat lastTime = 0.0f;
	GLfloat currentTime = glfwGetTime();
	GLfloat deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	// render
	// glClearColor(0.25f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	background.shader.use();
	background.shader.setTexture2D("u_Texture", background.texture->id);
	background.shader.setVec2("flip", 1.0f, 1.0f);
	background.shader.setBool("u_UseTexture", background.texture->loaded);
	background.shader.setFloat("u_Time", glfwGetTime());
	background.draw();

	sprite.shader.use();
	sprite.ssPlayer->update(deltaTime);	//Player update
	sprite.draw();	//Draw a layer

	glm::mat4 view;
	view = glm::translate(view, mover);
	view = glm::scale(view, scale*glm::vec3(1.0f, aspect_ratio, 1.0f));
	sprite.shader.setMat4("u_View", glm::value_ptr(view));
	sprite.shader.setBool("u_ButtonPressed", wireframe);

	sprite.shader.setFloat("u_Time", glfwGetTime());

	//glfw: swap buffers and poll IO events (keys pressed/releaed, mouse moved etc.)
	glfwSwapBuffers(window);
}

void zoom(double delta) {
	double z = delta*scale.y*2.0f;
	double sign = (scale.x > 0.0f) ? 1.0f : ((scale.x < 0.0f) ? -1.0f : 0.0f);
	scale += glm::vec3(z*sign, z, 0.0f);
	double threshold = 0.001f;
	if (scale.y < threshold) {
		scale.x = threshold * sign;
		scale.y = threshold;
	}
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	zoom(yoffset*0.06);
}

void handleEvents(GLFWwindow* window) {
	glfwPollEvents();
	// camera panning with mouse
	glm::dvec2 mousePos;
	glm::dvec2 deltaPos;
	glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
	static glm::dvec2 lastPos = mousePos;

	int left_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int right_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (left_button == GLFW_PRESS || right_button == GLFW_PRESS) {
		glm::ivec2 size;
		glfwGetFramebufferSize(window, &size.x, &size.y);
		deltaPos = mousePos - lastPos;
		lastPos = mousePos;
		deltaPos /= glm::dvec2(size.x, -size.y);
		if (left_button == GLFW_PRESS) {
			mover += glm::vec3(deltaPos*2.0, 0.0f);
		}
		if (right_button == GLFW_PRESS) {
			zoom(deltaPos.y);
		}
		return;
	}
	lastPos = mousePos;
}

void flip_view() {
	if (background.texture->loaded) {
		background.shader.use();
		background.shader.setVec2("flip", 1.0f, -1.0f);
	}
	sprite.shader.use();
	glm::mat4 view;
	scale.y = -scale.y;
	mover.y = -mover.y;
	view = glm::translate(view, mover);
	view = glm::scale(view, scale*glm::vec3(1.0f, aspect_ratio, 1.0f));
	sprite.shader.setMat4("u_View", glm::value_ptr(view));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifier) {
	switch (key) {
	case GLFW_KEY_A: case GLFW_KEY_LEFT:
		if (action == GLFW_PRESS) {
			sprite.next_anim();
			std::cout << '\n' << sprite.get_anim_name() << std::endl;
		}
		break;
	case GLFW_KEY_C:
		if (action == GLFW_PRESS) {
			mover = glm::vec3(0.0f, 0.0f, 0.0f);
			scale = glm::vec3(fivTwel, fivTwel, 1.0f);
		}
		break;
	case GLFW_KEY_H:
		if (action == GLFW_PRESS) {
			std::cout << help << std::endl;
		}
		break;
	case GLFW_KEY_L:
		if (action == GLFW_PRESS) {
			sprite.toggle_looping();
			std::cout << "Looping " << (sprite.is_looping() ? "enabled" : "disabled") << std::endl;
		}
		break;
	case GLFW_KEY_Q:
		if (action == GLFW_PRESS) { // screenshot

			int frame, max_frame;
			if (modifier == GLFW_MOD_SHIFT) {
				// export every frame of the current animation
				std::cout << "Exporting frames..." << std::endl;
				frame = 0;
				max_frame = sprite.ssPlayer->getMaxFrame();
			}
			else if (modifier == false) {
				// export single frame
				frame = sprite.ssPlayer->getFrameNo();
				max_frame = frame + 1;
			}
			else break;

			// create export directory
			std::string folder = (sprite.file_name + "_Screenshots" + "/");
			std::wstring wfolder;
			wfolder.assign(folder.begin(), folder.end());
			CreateDirectory(wfolder.c_str(), nullptr);
			folder = folder + sprite.ssPlayer->getPlayAnimeName() + "/";
			wfolder.assign(folder.begin(), folder.end());
			CreateDirectory(wfolder.c_str(), nullptr);

			// get viewport x, y, width & height //store it in vp[4]
			GLint vp[4];
			glGetIntegerv(GL_VIEWPORT, vp);
			GLubyte* image = new GLubyte[vp[2] * vp[3] * 4];
			std::string image_name;

			// flip the viewport first else the image(s) will be saved upside down
			flip_view();

			float delta = 1.0f / 30.0f;
			sprite.ssPlayer->setGameFPS(30.0f); // lock framerate to 30
			if (!sprite.is_paused()) { sprite.pause(); }
			do {
				glClear(GL_COLOR_BUFFER_BIT);

				if (background.texture->loaded) {
					background.shader.use();
					background.shader.setTexture2D("u_Texture", background.texture->id);
					background.draw();
				}

				// update sprite frame
				sprite.shader.use();
				sprite.ssPlayer->setFrameNo(frame++);
				sprite.ssPlayer->update(delta);
				sprite.draw();

				image_name = sprite.ssPlayer->getPlayAnimeName() + '_' + std::to_string(sprite.ssPlayer->getFrameNo()) + ".png";

				// get pixels from OpenGL and save to file
				glReadPixels(vp[0], vp[1], vp[2], vp[3], GL_RGBA, GL_UNSIGNED_BYTE, image);
				stbi_write_png((folder + image_name).c_str(), vp[2], vp[3], 4, image, vp[2] * 4);
			} while (frame < max_frame);

			delete[] image;

			sprite.ssPlayer->setGameFPS(frame_rate); //reset framerate
			flip_view(); // unflip viewport
			if (modifier == GLFW_MOD_SHIFT) std::cout << max_frame << " images saved to " << folder << std::endl;
			else { std::cout << "Image saved: " << folder + image_name << std::endl; }
		}
		break;
	case GLFW_KEY_S: case GLFW_KEY_RIGHT:
		if (action == GLFW_PRESS) {
			sprite.previous_anim();
			std::cout << '\n' << sprite.get_anim_name() << std::endl;
		}
		break;
	case GLFW_KEY_W:
		if (action == GLFW_PRESS) {
			static GLenum drawMode = GL_FILL;
			drawMode = (drawMode == GL_LINE) ? GL_FILL : GL_LINE;
			glPolygonMode(GL_FRONT_AND_BACK, drawMode);
			wireframe = !wireframe;
		}
		break;
	case GLFW_KEY_X:
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			scale.x *= -1.0;
		break;
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) {
			if (sprite.is_looping() == false) {
				sprite.replay();
				break;
			}
			if (sprite.is_paused()) {
				sprite.unpause();
				std::cout << "Playing\n";
			}
			else {
				sprite.pause();
				std::cout << "Paused\n";
			}
		}
		break;
	case GLFW_KEY_1:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			float play_speed = sprite.get_play_speed();
			if (play_speed <= -2.0f) break;
			sprite.set_play_speed(play_speed - 0.1f);
		}
		else if (action == GLFW_RELEASE)
			std::cout << setprecision(1) << std::fixed << "Play speed: " << sprite.get_play_speed() << '\n';
		break;
	case GLFW_KEY_2:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			float play_speed = sprite.get_play_speed();
			if (play_speed >= 2.0f) break;
			sprite.set_play_speed(play_speed + 0.1f);
			sprite.ssPlayer->setStep(play_speed);
		}
		else if (action == GLFW_RELEASE)
			std::cout << setprecision(1) << std::fixed << "Play speed: " << sprite.get_play_speed() << '\n';
		break;
	case GLFW_KEY_3:
		if (action == GLFW_PRESS) {
			sprite.set_play_speed(1.0f);
			std::cout << "Play speed reset\n";
		}
		break;
	case GLFW_KEY_PERIOD: case GLFW_KEY_UP:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			unsigned int max_frame = sprite.ssPlayer->getMaxFrame() - 1;
			sprite.pause();
			if (modifier == GLFW_MOD_SHIFT) {
				sprite.ssPlayer->setFrameNo(max_frame);
				break;
			}
			unsigned int current_frame = sprite.ssPlayer->getFrameNo();
			sprite.ssPlayer->setFrameNo(current_frame + (current_frame < max_frame));
		}
		break;
	case GLFW_KEY_COMMA: case GLFW_KEY_DOWN:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			sprite.pause();
			if (modifier == GLFW_MOD_SHIFT) {
				sprite.ssPlayer->setFrameNo(0);
				break;
			}
			unsigned int current_frame = sprite.ssPlayer->getFrameNo();
			sprite.ssPlayer->setFrameNo(current_frame - (current_frame > 0));
		}
		break;
	}
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	int i;
	for (i = 0; i < count; i++)
		//handle_dropped_file(paths[i]);
		std::cout << paths[i] << std::endl;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	aspect_ratio = float(width) / height;
	glViewport(0, 0, width, height);
	draw(window);
}
