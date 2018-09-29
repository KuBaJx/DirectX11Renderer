#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

void InputClass::Init()
{
	// Set all keys to released state
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int key)
{
	// If key is pressed save it's state to m_keys array
	if (!m_keys[key])
	{
		m_keys[key] = true;
	}
	return;
}

void InputClass::KeyUp(unsigned int key)
{
	if (m_keys[key])
	{
		m_keys[key] = false;
	}
	return;
}

bool InputClass::IsKeyDown(unsigned int key) const
{
	// Return state of desired key
	return m_keys[key];
}