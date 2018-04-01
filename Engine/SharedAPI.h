#pragma once

#define MYSHARED_API __declspec(dllexport)

extern "C"
{
	MYSHARED_API int Move(int move, double time_left, int current_level);
}

class SharedAPI
{
public:
	SharedAPI();
	~SharedAPI();
};
