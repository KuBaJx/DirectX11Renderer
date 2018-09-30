#include "CInput.h"

CInput::CInput()
{
}

CInput::CInput(const CInput& other)
{
}

CInput::~CInput()
{
}

void CInput::Init()
{
	// Set all keys to released state
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void CInput::KeyDown(unsigned int key)
{
	// If key is pressed save it's state to m_keys array
	if (!m_keys[key])
	{
		m_keys[key] = true;
	}
	return;
}

void CInput::KeyUp(unsigned int key)
{
	if (m_keys[key])
	{
		m_keys[key] = false;
	}
	return;
}

bool CInput::IsKeyDown(unsigned int key) const
{
	// Return state of desired key
	return m_keys[key];
}