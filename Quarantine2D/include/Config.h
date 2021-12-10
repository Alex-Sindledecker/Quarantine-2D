#pragma once

#ifdef Q2D_EXPORT
#define Q2D_RENDERING_API __declspec(dllexport)
#else
#define Q2D_RENDERING_API __declspec(dllimport)
#endif