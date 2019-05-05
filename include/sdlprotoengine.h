#ifndef SDLPROTOENGINE_H
#define SDLPROTOENGINE_H

#include <vector>
#include <SDL2/SDL.h>
#include <chrono>

class SDLInit {
    static bool bInitialized;
    std::vector<SDL_Window*> vWindows;
    SDL_Renderer* pRenderer;

public:
    SDLInit(Uint32 flags);
    ~SDLInit();

    bool IsInitialized();

    SDL_Window* CreateWindow(const std::string& wTitle, int wWidth, int wHeight);
    SDL_Renderer* CreateRenderer(SDL_Window* window);
};

//bool SDLInit::bInitialized = false;


class Chronometer {
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> start;

public:
    /*
     * Chreating a new chronometer automatically calls Reset() on start
     */
    Chronometer();

    /*
     * Reset chronometer
     */
    void Reset();

    /*
     * Return time elapsed since last Reset() call in milliseconds
     */
    Uint64 Elapsed();
};

struct ScheduleFlags
{
	bool windowTitle;
	bool windowWidth;
	bool windowHeight;

	static ScheduleFlags All();
	static ScheduleFlags None();

	ScheduleFlags WindowTitle();
	ScheduleFlags WindowWidth();
	ScheduleFlags WindowHeight();
};


class WorldState {
    bool bRunning;
    ScheduleFlags scheduleFlags;

public:
    WorldState();

    virtual void React(const SDL_Event& event);
    virtual void Update(Uint32 elapsed);
    virtual void Render(const SDL_Window* window, SDL_Renderer* renderer);

    bool IsRunning();

    ScheduleFlags PopScheduled();
    void Schedule(ScheduleFlags flags);

    virtual std::string WindowTitle();
    virtual Uint32 WindowWidth();
    virtual Uint32 WindowHeight();
    virtual Uint32 Fps();

protected:
    void Stop();
};

Uint64 loop(WorldState& worldState);

#endif