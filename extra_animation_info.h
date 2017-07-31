#ifndef EXTRA_DATA_H
#define EXTRA_DATA_H

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>

std::vector<std::string> readTheFile(const char* file_input);

namespace ss_extra
{
	/*
	* Definition of data contained in ss_extrabp
	*/

	/**
	* Type definition
	*/
	typedef short ss_extra_s16;
	typedef unsigned short ss_extra_u16;
	typedef unsigned int ss_extra_u32;
	typedef int ss_extra_32;
	typedef int ss_extra_offset;

	/**
	* Cell Map
	*/
	struct CellMap
	{
		ss_extra_offset name; // const char *
		ss_extra_offset imagePath; // const char *
		ss_extra_s16 index;
		ss_extra_s16 wrapMode; // clamp=0, repeat=1, mirror=2
		ss_extra_s16 filterMode; // nearest=0, bilinear=1
		ss_extra_s16 reserved; //blank 0x0000
	};


	/**
	* Cell
	*/
	struct Cell
	{
		ss_extra_offset name; // const char *
		ss_extra_offset cellMap; // const CellMap *
		ss_extra_s16 indexInCellMap;
		ss_extra_s16 x;
		ss_extra_s16 y;
		ss_extra_u16 width;
		ss_extra_u16 height;
		ss_extra_s16 reserved;
		float pivot_X;
		float pivot_Y;
	};


	/**
	* Anime information
	*/
	struct AnimationData
	{
		ss_extra_offset name; // const char*
		ss_extra_offset defaultData; // const AnimationInitialData*
		ss_extra_offset frameData; // const ss_extra_s16*
		ss_extra_offset userData; // const ss_extra_s16*
		ss_extra_offset labelData; // const ss_extra_s16*
		ss_extra_s16 numFrames;
		ss_extra_s16 fps;
		ss_extra_s16 labelNum;
		ss_extra_s16 canvass_extraizeX;
		ss_extra_s16 canvass_extraizeY;
	};


	/**
	* Initial value of each part
	*/
	struct AnimationInitialData
	{
		ss_extra_s16 index;
		ss_extra_s16 dummy;
		ss_extra_u32 flags;
		ss_extra_s16 cellIndex;
		ss_extra_s16 positionX;
		ss_extra_s16 positionY;
		ss_extra_s16 positionZ;
		ss_extra_u16 opacity;
		ss_extra_s16 dummy2;
		float anchorX;
		float anchorY;
		float rotationX;
		float rotationY;
		float rotationZ;
		float scaleX;
		float scaleY;
		float size_X;
		float size_Y;
		float uv_move_X;
		float uv_move_Y;
		float uv_rotation;
		float uv_scale_X;
		float uv_scale_Y;
		float boundingRadius;
	};

	/**
	* Parts
	*/
	struct PartData
	{
		ss_extra_offset name; /// const char * name of part/label/node
		ss_extra_s16 index; /// part index in ss_extra
		ss_extra_s16 parentIndex; /// parent's part index
		ss_extra_s16 type; /// part type: null=0, normal, text, instance, effect
		ss_extra_s16 boundsType; // / hit determination type: none=0, quad, aabb, circle, circle_smin(scale uses min), circle_smax
		ss_extra_s16 alphaBlendType; /// BlendType mix=0, multiplication=1, addition=2, subtraction=3
							   //ss_extra_s16 blank_space_here = 0x0000;
		ss_extra_offset refName; /// animated name placed as a const char * instance// Instance animation name
		ss_extra_offset effectName; // const char* Effect name
		ss_extra_offset partColor; // const char* name of label color: "Orange", "Green", "Violet" etc...
	};

	/**
	* Animation pack information
	*/
	struct AnimePackData
	{
		ss_extra_offset name; // const char *
		ss_extra_offset parts; // const PartData *
		ss_extra_offset animations; // const AnimationData *
		ss_extra_s16 numParts;
		ss_extra_s16 numAnimations;
	};

	/**
	* Project information
	*/
	struct ProjectData
	{
		ss_extra_u32 dataId;
		ss_extra_u32 version;
		ss_extra_u32 flags;
		ss_extra_offset imageBaseDir; // const char *
		ss_extra_offset cells; // const Cell *
		ss_extra_offset animePacks; // const AnimePackData *
		ss_extra_offset unknown; // eof? *
		ss_extra_s16 numCells;
		ss_extra_s16 numAnimePacks;
	};

}; // namespace ss_extra

void readProjectData(std::ifstream& file, ss_extra::ProjectData& projectData);
void readCell(std::ifstream& file, ss_extra::Cell& cell);
void readAnimPack(std::ifstream& file, ss_extra::AnimePackData& animPack);
void readPartData(std::ifstream& file, ss_extra::PartData& data);
void readAnimationData(std::ifstream& file, ss_extra::AnimationData& data);
void readAnimationInitialData(std::ifstream& file, ss_extra::AnimationInitialData& data);
std::string readString(std::ifstream& file, ss_extra::ss_extra_offset string_location);

struct ss_extrabpFile {
public:
	ss_extrabpFile() {}
	ss_extrabpFile(const std::string file_name) {
		file.open(file_name, std::ios_base::binary);
		init();
	}

	// DATA TYPES
	std::ifstream file;
	ss_extra::ProjectData projectData;
	std::vector<ss_extra::Cell> cellList;
	std::map<ss_extra::ss_extra_offset, ss_extra::CellMap> cellMapData; //TODO make maps instead of vectors
	std::vector<ss_extra::AnimePackData> animationPackList;
	//std::vector<ss_extra::AnimePackData> PackData;
	std::map<ss_extra::ss_extra_offset, std::vector<ss_extra::PartData>> partDataMap; // AnimePackData::parts is the key
	std::map<ss_extra::ss_extra_offset, std::vector<ss_extra::AnimationData>> animDataMap; // AnimePackData::animations is the key
	std::map<ss_extra::ss_extra_offset, std::vector<ss_extra::AnimationInitialData>> animationInitDataList; // AnimePackData::defaultData is the key

private:
	void init();
	void ss_extrabpFile::fillCells();
	void ss_extrabpFile::fillAnimPacks();
	ss_extra::CellMap ss_extrabpFile::readCellMap(ss_extra::ss_extra_offset location);
};

#endif