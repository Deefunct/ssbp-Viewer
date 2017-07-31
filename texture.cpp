#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::openFile() {
	// load and generate the texture
	unsigned char* pixel_array;
	FileReader file_data{ file_name };

	switch (file_data.type) {
	case FileReader::Type::OTHER: case FileReader::Type::PNG:
		if (file_data.size == 0) {
			retry();
			return;
		}
		//stbi_set_flip_vertically_on_load(true);
		pixel_array = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(file_data.c_str()),
			file_data.size, &width, &height, &nrChannels, 0);
		break;
	case FileReader::Type::WEBP:
		pixel_array = WebPDecodeRGBA(reinterpret_cast<const uint8_t*>(file_data.c_str()), file_data.size, &width, &height);
		nrChannels = 4;
		break;
	default:
		return;
	}

	// vertical flip code was copied from stb_image.h
	int w = width, h = height;
	int channels = nrChannels;
	int row, col, z;
		
	for (row = 0; row < (h >> 1); row++) {
		for (col = 0; col < w; col++) {
			for (z = 0; z < channels; z++) {
				char temp = pixel_array[(row * w + col) * channels + z];
				pixel_array[(row * w + col) * channels + z] = pixel_array[((h - row - 1) * w + col) * channels + z];
				pixel_array[((h - row - 1) * w + col) * channels + z] = temp;
			}
		}
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// wrapping & filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLenum format;
	switch (nrChannels) {
	case 1:
		format = GL_RED;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_RGB;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixel_array);
	glGenerateMipmap(GL_TEXTURE_2D);

	loaded = true;

	if (file_data.type == FileReader::Type::WEBP) {
		WebPFree((void*)pixel_array);
		return;
	}
	stbi_image_free((void*)pixel_array);
}

void Texture::retry() {
	if (times_failed < 3 && last_slash != 2) {
		if (times_failed == 0) {
			last_slash = file_name.find_last_of("/\\");
			file = file_name.substr(last_slash + 1);
		}
		last_slash = file_name.find_last_of("/\\", last_slash - 1);
		file_name = file_name.substr(0, last_slash + 1) + file;
		++times_failed;
		openFile();
	}
	else if ( message_on_fail ) {
		std::cout << "\nFailed to load texture:  " << file << std::endl;
	}
}

namespace Test {
void textureSettings7() {
	// Wrap modes GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// Texture filtering without mipmap filtering: GL_NEAREST, GL_LINEAR
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Texture/MipMap filtering: GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
};
