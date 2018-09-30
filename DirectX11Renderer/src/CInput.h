#pragma once
class CInput
{
public:
	CInput();
	CInput(const CInput&);
	~CInput();

	void Init();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int) const;

private:
	bool m_keys[256];
};

