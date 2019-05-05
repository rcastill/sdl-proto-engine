#include "sdlprotoengine.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <chrono>

#define DEFAULT_W_WIDTH 1920
#define DEFAULT_W_HEIGHT 1080
#define DEFAULT_FPS 1
#define DEFAULT_W_TITLE "SDL Tutorial"

#define LOG_SDL_ERROR() \
    (std::cerr << SDL_GetError() << std::endl)

#define LOG_INFO (std::cerr << "[info] ")

bool SDLInit::bInitialized = false;

SDLInit::SDLInit(Uint32 flags)
{
    if (bInitialized) {
        return;
    }
    if (SDL_Init(flags) < 0) {
        bInitialized = false;
    } else {
        pRenderer = NULL;
        bInitialized = true;
    }
};

SDLInit::~SDLInit()
{
    for (Uint32 i = 0; i < vWindows.size(); i++) {
        LOG_INFO << "SDL_DestroyWindow(" << i << ")" << std::endl;
        SDL_DestroyWindow(vWindows[i]);
    }
    if (pRenderer != NULL) {
        LOG_INFO << "SDL_DestroyRenderer()" << std::endl;
        SDL_DestroyRenderer(pRenderer);
    }
    LOG_INFO << "SDL_Quit" << std::endl;
    SDL_Quit();
}

bool SDLInit::IsInitialized()
{
    return bInitialized;
}

SDL_Window* SDLInit::CreateWindow(const std::string& wTitle, int wWidth, int wHeight)
{
    SDL_Window* w = SDL_CreateWindow(wTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wWidth, wHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (w != NULL) {
        vWindows.push_back(w);
    }
    return w;
}

SDL_Renderer* SDLInit::CreateRenderer(SDL_Window* window)
{
    pRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return pRenderer;
}

Chronometer::Chronometer()
{
    Reset();
}

void Chronometer::Reset()
{
    start = std::chrono::steady_clock::now();
}

Uint64 Chronometer::Elapsed()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
}

ScheduleFlags ScheduleFlags::All()
{
	ScheduleFlags flags = {true};
	return flags;
}

ScheduleFlags ScheduleFlags::None()
{
	ScheduleFlags flags = {false};
	return flags;
}

ScheduleFlags ScheduleFlags::WindowTitle()
{
	windowTitle	= true;
	return *this;
}

ScheduleFlags ScheduleFlags::WindowWidth()
{
	windowWidth	= true;
	return *this;
}

ScheduleFlags ScheduleFlags::WindowHeight()
{
	windowHeight = true;
	return *this;
}

WorldState::WorldState() : bRunning(true), scheduleFlags(ScheduleFlags::None())
{}

void WorldState::React(const SDL_Event& event)
{
}

void WorldState::Update(Uint32 elapsed)
{
}

void WorldState::Render(const SDL_Window* window, SDL_Renderer* renderer)
{
}

bool WorldState::IsRunning()
{
	return bRunning;
}

void WorldState::Stop()
{
	bRunning = false;
}

ScheduleFlags WorldState::PopScheduled()
{
    ScheduleFlags ret = scheduleFlags;
    scheduleFlags = ScheduleFlags::None();
	return ret;
}

void WorldState::Schedule(ScheduleFlags flags)
{
	scheduleFlags = flags;
}

std::string WorldState::WindowTitle()
{
	return "Main Window";
}

Uint32 WorldState::WindowWidth()
{
	return 800;
}

Uint32 WorldState::WindowHeight()
{
	return 600;
}

Uint32 WorldState::Fps()
{
	return 30;
}

Uint64 loop(WorldState& worldState)
{
    Chronometer loopChronometer;

    SDLInit initObject(SDL_INIT_VIDEO);
    if (!initObject.IsInitialized()) {
        LOG_SDL_ERROR();
        return 1;
    }

    int wWidth = worldState.WindowWidth();
    int wHeight = worldState.WindowHeight();
    int fps = worldState.Fps();
    std::string wTitle = worldState.WindowTitle();

    LOG_INFO << "window dimensions: " << wWidth << "x" << wHeight << std::endl;
    LOG_INFO << "target fps: " << fps << std::endl;
    SDL_Window* mainWindow = initObject.CreateWindow(wTitle.c_str(), wWidth, wHeight);
    if (mainWindow == NULL) {
        LOG_SDL_ERROR();
        return 1;
    }

    SDL_Renderer* renderer = initObject.CreateRenderer(mainWindow);
    if (renderer == NULL) {
        LOG_SDL_ERROR();
        return 1;
    }

    bool exitPressed = false;
    SDL_Event ev;
    Chronometer chrono;
    Chronometer fpsChrono;
    int remainingTime;
    int const timePerFrame = 1000 / fps;
    int frameCount = 0;
    Uint64 frameElapsed = 0;

    while (!exitPressed && worldState.IsRunning()) {
        if (fpsChrono.Elapsed() >= 1000) {
            fpsChrono.Reset();
            //frameCount == fps
            frameCount = 0;
        }
        frameElapsed = chrono.Elapsed();
        chrono.Reset();
        while (SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                exitPressed = true;
            } else {
                worldState.React(ev);
            }
        }
        worldState.Update(frameElapsed);
        // black bg
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        worldState.Render(mainWindow, renderer);
        SDL_RenderPresent(renderer);

        frameCount++;

        ScheduleFlags schFlags = worldState.PopScheduled();
        if (schFlags.windowTitle)
        {
        	SDL_SetWindowTitle(mainWindow, worldState.WindowTitle().c_str());
        }

        if ((remainingTime = timePerFrame - chrono.Elapsed()) > 0) {
            SDL_Delay(remainingTime);
        } else {
            LOG_INFO << "running " << -remainingTime << "ms behind" << std::endl;
        }
    }

    return loopChronometer.Elapsed();
}