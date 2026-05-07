#include "texture.h"
#include "utils.h"

// 安全解码BMP，空指针直接返回
unsigned char* DecodeBMP(unsigned char* bmpFileData, int& width, int& height)
{
	// ====================== 关键修复：进来先判断 nullptr ======================
	if (bmpFileData == nullptr)
	{
		width = 0;
		height = 0;
		return nullptr;
	}

	if (0x4D42 == *((unsigned short*)bmpFileData))
	{
		int pixelDataOffset = *((int*)(bmpFileData + 10));
		width = *((int*)(bmpFileData + 18));
		height = *((int*)(bmpFileData + 22));
		unsigned char* pixelData = bmpFileData + pixelDataOffset;

		for (int i = 0; i < width * height * 3; i += 3)
		{
			unsigned char temp = pixelData[i];
			pixelData[i] = pixelData[i + 2];
			pixelData[i + 2] = temp;
		}
		return pixelData;
	}
	else
	{
		width = 0;
		height = 0;
		return nullptr;
	}
}

void Texture::Init(const char* imagePath)
{
	unsigned char* imageFileContent = LoadFileContent(imagePath);

	// ====================== 关键修复：加载失败直接退出 ======================
	if (imageFileContent == nullptr)
	{
		MessageBoxA(NULL, "图片加载失败！\n请检查文件路径是否正确", "错误", MB_OK);
		return;
	}

	int width = 0, height = 0;
	unsigned char* pixelData = DecodeBMP(imageFileContent, width, height);

	// ====================== 关键修复：解码失败也安全退出 ======================
	if (pixelData == nullptr || width <= 0 || height <= 0)
	{
		delete[] imageFileContent;
		MessageBoxA(NULL, "图片格式错误！只支持24位BMP", "错误", MB_OK);
		return;
	}

	// OpenGL纹理
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] imageFileContent;
}