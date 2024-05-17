#pragma once

#ifdef SIMULATION_DLL
#define SIMULATION_API __declspec(dllexport)
#else
#define SIMULATION_API __declspec(dllimport)
#endif