#include"textureclass.h"

TextureClass::TextureClass()
{
	m_TargaData = 0;
	m_Texture = 0;
	m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass &)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext, char *fileName)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	result = LoadTarga(fileName, height, width);
	if (!result) {
		MessageBox(NULL, L"error LoadTarga", NULL, MB_OK);
		return false;
	}

	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_Texture);
	if (FAILED(hResult)) {
		MessageBox(NULL, L"Error with create 2d", NULL, MB_OK);
		return false;
	}
	rowPitch = 4 * width * sizeof(unsigned char);
	deviceContext->UpdateSubresource(m_Texture, 0, NULL, m_TargaData, rowPitch, 0);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hResult = device->CreateShaderResourceView(m_Texture, &srvDesc, &m_textureView);
	if (FAILED(hResult)) {
		MessageBox(NULL, L"Error with resource view", NULL, MB_OK);
		return false;
	}
	deviceContext->GenerateMips(m_textureView);

	delete[] m_TargaData;
	m_TargaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = 0;
	}

	// Release the targa data.
	if (m_TargaData)
	{
		delete[] m_TargaData;
		m_TargaData = 0;
	}

	return;
}

ID3D11ShaderResourceView * TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga(char *fileName, int &height, int &width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* file;
	unsigned int count;
	TargaHeader header;
	unsigned char* targaImage;

	error = fopen_s(&file, fileName, "rb");
	if(error != 0)
	{
		MessageBox(NULL, L"failed to open file", NULL, MB_OK);
		return false;
	}

	count = (unsigned int)fread(&header, sizeof(TargaHeader), 1, file);
	if (count != 1)
	{
		return false;
	}

	bpp = header.bpp;
	width = header.width;
	height = header.height;

	if (bpp != 32) { MessageBox(NULL, L"Error with bpp", NULL, MB_OK); return false; }

	imageSize = width*height * 4;
	targaImage = new unsigned char[imageSize];
	if (!targaImage) { MessageBox(NULL, L"Error with image data", NULL, MB_OK); return false; }

	count = fread(targaImage, 1, imageSize, file);
	if (count != imageSize) { MessageBox(NULL, L"Error with reading again", NULL, MB_OK); return false; }

	m_TargaData = new unsigned char[imageSize];

	error = fclose(file);
	if (error != 0)
	{
		MessageBox(NULL, L"Error with closing again", NULL, MB_OK);
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j<height; j++)
	{
		for (i = 0; i<width; i++)
		{
			m_TargaData[index + 0] = targaImage[k + 2];  // Red.
			m_TargaData[index + 1] = targaImage[k + 1];  // Green.
			m_TargaData[index + 2] = targaImage[k + 0];  // Blue
			m_TargaData[index + 3] = targaImage[k + 3];  // Alpha

														 // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	delete[] targaImage;
	targaImage = 0;



	return true;
}
