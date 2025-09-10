#pragma once
#include <d3d12.h>

class RootSignature
{
public:
	virtual ~RootSignature() = default;
	virtual bool IsValid() const = 0;
	virtual ID3D12RootSignature* Get() = 0;
};