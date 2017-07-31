#include <iostream>
//#include <chrono>
//#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include "extra_animation_info.h"
#include "texture.h"
#include "quad.h"
#include "sprite.h"

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
bool button = false; // for wireframe toggle. true if spacebar is pressed
const float fivTwel = 1.0f / 256.0f; // for viewport scale
Texture* bg_Texture;
Quad background; // the background quad/plane; has its own shader
glm::vec3 mover(0.0f, -0.5f, 0.0f); // camera position
glm::vec3 scale(fivTwel, fivTwel, 1.0f); // camera view scale
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

Sprite sprites;

int main(int argc, char* argv[]) {
	// glfw: initialize and configure
	sprites.location = argv[0];
	sprites.location = sprites.location.substr(0, sprites.location.find_last_of("/\\") + 1);

	std::vector<std::string> shader_name_list{
		"shaders/sprite.vertex",
		"shaders/sprite.fragment",
		"shaders/background.vertex",
		"shaders/background.fragment"
	};
	
	if (argc == 1) {
		std::cout << "Drag an ssbp file here then press enter.\n";
		std::cin >> sprites.file_name;
	}
	else if (argc == 2) {
		sprites.file_name = argv[1];

		for (int i = 0; i < shader_name_list.size(); ++i) {
			shader_name_list[i] = sprites.location + shader_name_list[i];
		}

	}
	else return 0;

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
	sprites.init(shader_name_list[0], shader_name_list[1]);
	background.init(shader_name_list[2], shader_name_list[3]);
	bg_Texture = new Texture((sprites.location + "shaders/background.png").c_str(), false); // load background image


																							//glEnable(GL_CULL_FACE); //test

	sprites.resman = ss::ResourceManager::getInstance();
	sprites.ssPlayer = ss::Player::create();

	sprites.resman->addData(sprites.file_name);
	sprites.animation_list = readTheFile(sprites.file_name.c_str()); // retrieve animation names
	sprites.file_name = sprites.file_name.substr(sprites.file_name.find_last_of("/\\") + 1); // remove directory & slashes from string
	sprites.file_name = sprites.file_name.substr(0, sprites.file_name.find_last_of('.')); // remove .ssbp extension so only left with file's name
	sprites.ssPlayer->setData(sprites.file_name);
	sprites.ssPlayer->play(sprites.animation_list[0], 1);
	sprites.ssPlayer->setGameFPS(frame_rate);
	//ssPlayer->frame

	std::cout << help << '\n' << sprites.file_name << "\nNumber of animations: " << sprites.animation_list.size() << "\n\n" << sprites.ssPlayer->getPlayAnimeName() << std::endl;

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
	delete bg_Texture;
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
	background.shader.setTexture2D("u_Texture", bg_Texture->id);
	background.shader.setVec2("flip", 1.0f, 1.0f);
	background.shader.setBool("u_UseTexture", bg_Texture->loaded);
	background.shader.setFloat("u_Time", glfwGetTime());
	background.draw();

	sprites.shader.use();
	sprites.ssPlayer->update(deltaTime);	//Player update
	sprites.draw();	//Draw a layer

	glm::mat4 view;
	view = glm::translate(view, mover);
	view = glm::scale(view, scale*glm::vec3(1.0f, aspect_ratio, 1.0f));
	sprites.shader.setMat4("u_View", glm::value_ptr(view));
	sprites.shader.setBool("u_ButtonPressed", button);

	sprites.shader.setFloat("u_Time", glfwGetTime());

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
	if (bg_Texture->loaded) {
		background.shader.use();
		background.shader.setVec2("flip", 1.0f, -1.0f);
	}
	sprites.shader.use();
	glm::mat4 view;
	scale.y = -scale.y;
	mover.y = -mover.y;
	view = glm::translate(view, mover);
	view = glm::scale(view, scale*glm::vec3(1.0f, aspect_ratio, 1.0f));
	sprites.shader.setMat4("u_View", glm::value_ptr(view));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifier) {
	static int anim_index = 0;
	static float play_speed = 1.0f;
	static bool dont_loop = true;

	switch (key) {
	case GLFW_KEY_A: case GLFW_KEY_LEFT:
		if (action == GLFW_PRESS) {
			anim_index = (anim_index == 0) ? sprites.animation_list.size() - 1 : --anim_index;
			sprites.ssPlayer->play(sprites.animation_list[anim_index], dont_loop);
			sprites.ssPlayer->setStep(play_speed);
			std::cout << '\n' << sprites.ssPlayer->getPlayAnimeName() << std::endl;
			sprites.unpause();
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
			dont_loop = !dont_loop;
			sprites.ssPlayer->play(sprites.animation_list[anim_index], dont_loop, sprites.ssPlayer->getFrameNo());
			sprites.ssPlayer->setStep(play_speed);
			std::cout << "Looping " << (dont_loop ? "disabled" : "enabled") << std::endl;
			sprites.unpause();
		}
		break;
	case GLFW_KEY_Q:
		if (action == GLFW_PRESS) { // screenshot

			int frame, max_frame;
			if (modifier == GLFW_MOD_SHIFT) {
				// export every frame of the current animation
				std::cout << "Exporting frames..." << std::endl;
				frame = 0;
				max_frame = sprites.ssPlayer->getMaxFrame();
			}
			else if (modifier == false) {
				// export single frame
				frame = sprites.ssPlayer->getFrameNo();
				max_frame = frame + 1;
			}
			else break;

			// create export directory
			std::string folder = (sprites.file_name + "_Screenshots" + "/");
			std::wstring wfolder;
			wfolder.assign(folder.begin(), folder.end());
			CreateDirectory(wfolder.c_str(), nullptr);
			folder = folder + sprites.ssPlayer->getPlayAnimeName() + "/";
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
			sprites.ssPlayer->setGameFPS(30.0f); // lock framerate to 30
			if (!sprites.paused) { sprites.pause(); }
			do {
				glClear(GL_COLOR_BUFFER_BIT);

				if (bg_Texture->loaded) {
					background.shader.use();
					background.shader.setTexture2D("u_Texture", bg_Texture->id);
					background.draw();
				}

				// update sprite frame
				sprites.shader.use();
				sprites.ssPlayer->setFrameNo(frame++);
				sprites.ssPlayer->update(delta);
				sprites.draw();

				image_name = sprites.ssPlayer->getPlayAnimeName() + '_' + std::to_string(sprites.ssPlayer->getFrameNo()) + ".png";

				// get pixels from OpenGL and save to file
				glReadPixels(vp[0], vp[1], vp[2], vp[3], GL_RGBA, GL_UNSIGNED_BYTE, image);
				stbi_write_png((folder + image_name).c_str(), vp[2], vp[3], 4, image, vp[2] * 4);
			} while (frame < max_frame);

			delete[] image;

			sprites.ssPlayer->setGameFPS(frame_rate); //reset framerate
			flip_view(); // unflip viewport
			if (modifier == GLFW_MOD_SHIFT) std::cout << max_frame << " images saved to " << folder << std::endl;
			else { std::cout << "Image saved: " << folder + image_name << std::endl; }
		}
		break;
	case GLFW_KEY_S: case GLFW_KEY_RIGHT:
		if (action == GLFW_PRESS) {
			anim_index = (anim_index == sprites.animation_list.size() - 1) ? 0 : ++anim_index;
			sprites.ssPlayer->play(sprites.animation_list[anim_index], dont_loop);
			sprites.ssPlayer->setStep(play_speed);
			std::cout << '\n' << sprites.ssPlayer->getPlayAnimeName() << std::endl;
			sprites.unpause();
		}
		break;
	case GLFW_KEY_W:
		if (action == GLFW_PRESS) {
			static GLenum drawMode = GL_FILL;
			drawMode = (drawMode == GL_LINE) ? GL_FILL : GL_LINE;
			glPolygonMode(GL_FRONT_AND_BACK, drawMode);
			button = !button;
		}
		break;
	case GLFW_KEY_X:
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			scale.x *= -1.0;
		break;
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) {
			if (dont_loop) {
				sprites.ssPlayer->play(sprites.animation_list[anim_index]);
				sprites.ssPlayer->setStep(play_speed);
				sprites.ssPlayer->setLoop(1);
				break;
			}
			if (sprites.paused) {
				sprites.unpause();
				std::cout << "Paused\n";
			}
			else {
				sprites.pause();
				std::cout << "Playing\n";
			}
		}
		break;
	case GLFW_KEY_1:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			if (play_speed <= -2.0f) break;
			play_speed -= 0.1f;
			sprites.ssPlayer->setStep(play_speed);
		}
		else if (action == GLFW_RELEASE)
			std::cout << setprecision(1) << std::fixed << "Play speed: " << play_speed << '\n';
		break;
	case GLFW_KEY_2:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			if (play_speed >= 2.0f) break;
			play_speed += 0.1f;
			sprites.ssPlayer->setStep(play_speed);
		}
		else if (action == GLFW_RELEASE)
			std::cout << setprecision(1) << std::fixed << "Play speed: " << play_speed << '\n';
		break;
	case GLFW_KEY_3:
		if (action == GLFW_PRESS) {
			play_speed = 1.0f;
			sprites.ssPlayer->setStep(play_speed);
			std::cout << "Play speed reset\n";
		}
		break;
	case GLFW_KEY_PERIOD: case GLFW_KEY_UP:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			unsigned int max_frame = sprites.ssPlayer->getMaxFrame() - 1;
			sprites.pause();
			if (modifier == GLFW_MOD_SHIFT) {
				sprites.ssPlayer->setFrameNo(max_frame);
				break;
			}
			unsigned int current_frame = sprites.ssPlayer->getFrameNo();
			sprites.ssPlayer->setFrameNo(current_frame + (current_frame < max_frame));
		}
		break;
	case GLFW_KEY_COMMA: case GLFW_KEY_DOWN:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			sprites.pause();
			if (modifier == GLFW_MOD_SHIFT) {
				sprites.ssPlayer->setFrameNo(0);
				break;
			}
			unsigned int current_frame = sprites.ssPlayer->getFrameNo();
			sprites.ssPlayer->setFrameNo(current_frame - (current_frame > 0));
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
