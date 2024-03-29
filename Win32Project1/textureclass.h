#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include<string>
#include<d3d11.h>
#include<stdio.h>

class TextureClass
{
private:
	
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:

	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(char*, int&, int&);

	unsigned char* m_TargaData;
	ID3D11Texture2D* m_Texture;
	ID3D11ShaderResourceView* m_textureView;
};

#endif