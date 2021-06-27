#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class ImageInfo
{
public:
	ImageInfo(fs::path filePath);
	~ImageInfo();
	gl::Texture2dRef texture;
	std::string name;
	vec4 colorInfo;
};

ImageInfo::ImageInfo(fs::path filePath):colorInfo(vec4(1,1,1,1)){
	name = filePath.filename().string();
	texture = gl::Texture2d::create(loadImage(filePath));

}

ImageInfo::~ImageInfo() {}

#endif