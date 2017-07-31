#include "extra_animation_info.h"


std::string readString(std::ifstream& file, ss_extra::ss_extra_offset string_location)
{
	std::streampos initial_pos = file.tellg();
	file.seekg(string_location);
	char ch;
	std::string text;
	while (file.get(ch)) {
		if (ch == 0x00) break;
		text += ch;
	}
	file.seekg(initial_pos);
	return std::string{ text };
}

void ss_extrabpFile::init()
{
	if (!file) { std::cout << "NO FILE\n"; return; }
	readProjectData(file, projectData); // ProjectData

	file.seekg(projectData.cells);
	cellList.resize(projectData.numCells);
	fillCells(); // Cell & CellMap

	file.seekg(projectData.animePacks);
	animationPackList.resize(projectData.numAnimePacks);
	fillAnimPacks(); // AnimePackData
}

void ss_extrabpFile::fillCells() {
	for (int i = 0; i < projectData.numCells; ++i) {
		readCell(file, cellList[i]);
		std::map<ss_extra::ss_extra_offset, ss_extra::CellMap>::iterator it = cellMapData.find(cellList[i].cellMap);
		if (it == cellMapData.end())
			cellMapData[cellList[i].cellMap] = readCellMap(cellList[i].cellMap);
	}
}

void ss_extrabpFile::fillAnimPacks() {
	for (int i = 0; i < projectData.numAnimePacks; ++i) {
		readAnimPack(this->file, animationPackList[i]);
		std::streampos last_pos = file.tellg();


		std::vector<ss_extra::PartData> partsTemp;
		partsTemp.resize(animationPackList[i].numParts);
		file.seekg(animationPackList[i].parts);
		for (int j = 0; j < animationPackList[i].numParts; ++j) {
			readPartData(file, partsTemp[j]);
		}
		partDataMap[animationPackList[i].parts] = partsTemp;

		std::vector<ss_extra::AnimationData> AnimsTemp;
		AnimsTemp.resize(animationPackList[i].numAnimations);
		file.seekg(animationPackList[i].animations);
		for (int j = 0; j < animationPackList[i].numAnimations; ++j) {
			readAnimationData(file, AnimsTemp[j]);
			std::streampos ip = file.tellg();
			std::vector<ss_extra::AnimationInitialData> testing;
			testing.resize(animationPackList[i].numParts);
			file.seekg(AnimsTemp[j].defaultData);
			for (ss_extra::AnimationInitialData& aData : testing) {
				readAnimationInitialData(file, aData);
			}
			animationInitDataList[AnimsTemp[j].defaultData] = testing;
			file.seekg(ip);
		}
		animDataMap[animationPackList[i].animations] = AnimsTemp;


		file.seekg(last_pos);
	}
}

void readProjectData(std::ifstream& file, ss_extra::ProjectData& projectData)
{
	file.read(reinterpret_cast<char*>(&projectData.dataId), sizeof(ss_extra::ss_extra_u32));
	file.read(reinterpret_cast<char*>(&projectData.version), sizeof(ss_extra::ss_extra_u32));
	file.read(reinterpret_cast<char*>(&projectData.flags), sizeof(ss_extra::ss_extra_u32));
	file.read(reinterpret_cast<char*>(&projectData.imageBaseDir), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&projectData.cells), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&projectData.animePacks), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&projectData.unknown), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&projectData.numCells), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&projectData.numAnimePacks), sizeof(ss_extra::ss_extra_s16));
}

void readCell(std::ifstream& file, ss_extra::Cell& cell) {
	file.read(reinterpret_cast<char*>(&cell.name), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&cell.cellMap), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&cell.indexInCellMap), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&cell.x), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&cell.y), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&cell.width), sizeof(ss_extra::ss_extra_u16));
	file.read(reinterpret_cast<char*>(&cell.height), sizeof(ss_extra::ss_extra_u16));
	file.read(reinterpret_cast<char*>(&cell.reserved), sizeof(ss_extra::ss_extra_s16)); //dummy data
	file.read(reinterpret_cast<char*>(&cell.pivot_X), sizeof(float));
	file.read(reinterpret_cast<char*>(&cell.pivot_Y), sizeof(float));
}

void readAnimPack(std::ifstream& file, ss_extra::AnimePackData& animPack)
{
	file.read(reinterpret_cast<char*>(&animPack.name), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&animPack.parts), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&animPack.animations), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&animPack.numParts), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&animPack.numAnimations), sizeof(ss_extra::ss_extra_s16));
}

ss_extra::CellMap ss_extrabpFile::readCellMap(ss_extra::ss_extra_offset cellMapLocation) {
	std::streampos initial_pos = file.tellg();
	file.seekg(cellMapLocation);

	ss_extra::CellMap temp;
	file.read(reinterpret_cast<char*>(&temp.name), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&temp.imagePath), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&temp.index), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&temp.reserved), sizeof(ss_extra::ss_extra_s16));
	file.seekg(initial_pos);
	return temp;
}

void readPartData(std::ifstream& file, ss_extra::PartData& data) {
	file.read(reinterpret_cast<char*>(&data.name), sizeof(ss_extra::ss_extra_offset)); //const char *
	file.read(reinterpret_cast<char*>(&data.index), sizeof(ss_extra::ss_extra_s16)); //part index in ss_extra
	file.read(reinterpret_cast<char*>(&data.parentIndex), sizeof(ss_extra::ss_extra_s16)); // parent's part index
	file.read(reinterpret_cast<char*>(&data.type), sizeof(ss_extra::ss_extra_s16)); // part type
	file.read(reinterpret_cast<char*>(&data.boundsType), sizeof(ss_extra::ss_extra_s16)); // / hit determination type
	file.read(reinterpret_cast<char*>(&data.alphaBlendType), sizeof(ss_extra::ss_extra_s16)); // BlendType
	file.seekg(file.tellg() + std::streampos(2));
	file.read(reinterpret_cast<char*>(&data.refName), sizeof(ss_extra::ss_extra_offset)); // animated name placed as a const char * instance
	file.read(reinterpret_cast<char*>(&data.effectName), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&data.partColor), sizeof(ss_extra::ss_extra_offset));
}


void readAnimationData(std::ifstream& file, ss_extra::AnimationData& data) {
	file.read(reinterpret_cast<char*>(&data.name), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&data.defaultData), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&data.frameData), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&data.userData), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&data.labelData), sizeof(ss_extra::ss_extra_offset));
	file.read(reinterpret_cast<char*>(&data.numFrames), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.fps), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.labelNum), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.canvass_extraizeX), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.canvass_extraizeY), sizeof(ss_extra::ss_extra_s16));
	file.seekg(file.tellg() + std::streampos(2));
}

void readAnimationInitialData(std::ifstream& file, ss_extra::AnimationInitialData& data) {
	file.read(reinterpret_cast<char*>(&data.index), sizeof(ss_extra::ss_extra_s16));
	file.seekg(file.tellg() + std::streampos(2));
	file.read(reinterpret_cast<char*>(&data.flags), sizeof(ss_extra::ss_extra_u32));
	file.read(reinterpret_cast<char*>(&data.cellIndex), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.positionX), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.positionY), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.positionZ), sizeof(ss_extra::ss_extra_s16));
	file.read(reinterpret_cast<char*>(&data.opacity), sizeof(ss_extra::ss_extra_u16));
	file.seekg(file.tellg() + std::streampos(2));
	file.read(reinterpret_cast<char*>(&data.anchorX), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.anchorY), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.rotationX), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.rotationY), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.rotationZ), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.scaleX), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.scaleY), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.size_X), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.size_Y), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.uv_move_X), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.uv_move_Y), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.uv_rotation), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.uv_scale_X), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.uv_scale_Y), sizeof(float));
	file.read(reinterpret_cast<char*>(&data.boundingRadius), sizeof(float));
}

std::vector<std::string> readTheFile(const char* file_input) {
	std::string File_Name;
	std::string File_Dir;
	std::string input = file_input;
	std::vector<std::string> animation_list;
	if (input[0] == '"') {
		input = input.substr(1, input.length() - 2);
	}
	std::size_t last_slash = input.find_last_of("/\\");
	if (last_slash != std::string::npos) {
		File_Name = input.substr(last_slash + 1);
		File_Dir = input.substr(0, last_slash + 1);
	}
	ss_extrabpFile test{ input };

	for (ss_extra::AnimePackData animPack : test.animationPackList) {
		std::vector<ss_extra::AnimationData>& animDataList = test.animDataMap[animPack.animations];


		for (int anim_indx = 0; anim_indx < animPack.numAnimations; ++anim_indx) {
			animation_list.push_back(readString(test.file, animPack.name) + '/' + readString(test.file, animDataList[anim_indx].name));
			//<< std::uppercase << std::hex << "Pack: " << readString(test.file, animPack.name) << '\n'
			//<< std::uppercase << std::hex << "Name: " << readString(test.file, animDataList[anim_indx].name) << '\n';

		}
	}
	return animation_list;
}
