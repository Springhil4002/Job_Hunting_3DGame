#pragma once
#include <d3dx12.h>
#include "CommonTypes.h"
#include "ComPtr.h"

struct Vertex
{
	XMFLOAT3 position;	
	XMFLOAT3 normal;	
	XMFLOAT2 uv;		
	XMFLOAT3 tangent;	
	XMFLOAT4 color;		

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 5;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexInstance
{
	XMFLOAT3 position;	
	XMFLOAT3 normal;	
	XMFLOAT2 uv;		
	XMFLOAT3 tangent;	
	XMFLOAT4 color;		

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 8;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) Matrix
{
	Matrix4x4 world;	
	Matrix4x4 view;		
	Matrix4x4 proj;		
	float time;			
	XMFLOAT3 cameraPos;	
	float alpha;		
};

struct alignas(256) MatrixUI
{
	Matrix4x4 world;	
	Matrix4x4 view;		
	Matrix4x4 proj;		
	float alpha;		
	float padding[3];	
	XMFLOAT4 uv;		
};

struct GerstnerParams
{
	DirectX::XMFLOAT4 amplitude[4];	
	DirectX::XMFLOAT4 direction[4];	
	DirectX::XMFLOAT4 waveLength[4];
	DirectX::XMFLOAT4 speed[4];		
};

struct Mesh
{
	std::vector<Vertex> Vertices;	
	std::vector<uint32_t> Indices;	
	std::wstring DiffuseMap;		
};

struct LightPalams
{
	XMFLOAT3 lightDir;		
	float envStrength;		
	XMFLOAT4 lightColor;	
};

struct SkyVertex {
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 2;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexUI
{
	XMFLOAT3 pos;	
	XMFLOAT2 uv;	
	XMFLOAT4 color;	
	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 3;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct Mesh_UI
{
	std::vector<VertexUI> vertices;	
	std::vector<uint32_t> indices;	
	std::wstring DiffuseMap;		
};

struct WaterParams
{
	XMFLOAT2 fbmScale;       
	float fbmGain;            
	float fbmLacunarity;      

	int fbmOctaves;            
	float normalPerturb;       
	float foamThreshold;       
	float foamIntensity;       

	XMFLOAT4 shallowColor;     
	XMFLOAT4 deepColor;        
};