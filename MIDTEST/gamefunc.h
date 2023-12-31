#pragma once

#include <iostream>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

void InitGame();
void ClearGame();

//Game Phases
const int PHASE_INTRO = 0;
const int PHASE_STAGE1 = 1;
const int PHASE_ENDING = 2;

extern int g_current_game_phase;
extern bool g_flag_running;
extern SDL_Renderer* g_renderer;
extern SDL_Window* g_window;

class PhaseInterface {
public:
	virtual void HandleEvents() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Clear() = 0;
};