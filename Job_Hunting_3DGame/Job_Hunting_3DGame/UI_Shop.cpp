#include "UI_Shop.h"
#include "Debug_New.h"

std::unique_ptr<Object> UI_Shop::clone() const
{
	auto newObj = std::make_unique<UI_Shop>();
	return newObj;
}



void UI_Shop::Update()
{
	
}

void UI_Shop::Draw()
{

}

void UI_Shop::UnInit()
{

}