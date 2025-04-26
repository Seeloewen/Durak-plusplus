#pragma once

#include <qimage.h>
#include <string>
#include <map>

inline std::unordered_map<std::string, QImage*> textures;

void initTextures();
QImage* getTexture(std::string id);
void cleanUpTextures();