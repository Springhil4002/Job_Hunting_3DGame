#include "Object.h"

DirectX::XMVECTOR Object::GetPos() const 
{
	return XMLoadFloat3(&m_Position);
}

DirectX::XMVECTOR Object::GetRota() const
{
	return XMLoadFloat3(&m_Rotation);
}

DirectX::XMVECTOR Object::GetScale() const
{
	return XMLoadFloat3(&m_Scale);
}

void Object::SetPos(DirectX::XMVECTOR _pos)
{
	DirectX::XMStoreFloat3(&m_Position, _pos);
}

void Object::SetRota(DirectX::XMVECTOR _rota)
{
	DirectX::XMStoreFloat3(&m_Rotation, _rota);
}

void Object::SetScale(DirectX::XMVECTOR _scale)
{
	DirectX::XMStoreFloat3(&m_Scale, _scale);
}