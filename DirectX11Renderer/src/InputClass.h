#pragma once
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Init();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int) const;

private:
	bool m_keys[256];
};

