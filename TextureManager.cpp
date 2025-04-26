#include <fstream>
#include <nlohmann/json.hpp>
#include <qimage.h>
#include "TextureManager.h"
#include <string>
#include <LogUtil.h>

using json = nlohmann::json;

void initTextures()
{
	std::ifstream file;
	file.open("textures/textures.json");

	if (!file.is_open())
	{
		logE("Could not initialize textures. Make sure the textures.json file exists.");
		return;
	}

	json j = json::parse(file);

	for (const json& texture : j["textures"])
	{

		std::string imagePath = std::format("textures/{}", static_cast<std::string>(texture["file"]));
		QImage* image = new QImage(imagePath.c_str());
		std::string id = texture["id"];

		textures[id] = image;
	}

	logI("Successfully initialized textures");
}

QImage* getTexture(std::string id)
{
	return textures[id];
}

void cleanUpTextures()
{
	for (const std::pair<std::string, QImage*> entry : textures)
	{
		delete entry.second;
	}
	textures.clear();
}