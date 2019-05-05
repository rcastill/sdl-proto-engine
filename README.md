# SDL2 Prototype Engine

Barebones primitives to quickly prototype graphical applications using SDL2. It mainly provides event handling and loop lifecycle.

## Compilation

In command line:

```
$ git clone github.com/rcastill/sdl-proto-engine.git
$ cd sdl-proto-engine
$ mkdir build
$ cd build
$ cmake ..
$ make
```

It is also useful to install headers/lib in your system:

```
$ [sudo] make install
```

Remember to check `install_manifest.txt` under sdl-proto-engine/build in case you want to remove installed targets

## Example

```c++
/* file: example.cc */
#include <sdlprotoengine.h>
#include <string>

class World : public WorldState
{
	Uint64 elapsedUntil;
	int secsElapsed;
public:
	World();

	/* Called on init and everytime it's scheduled */
	std::string WindowTitle();
	Uint32 Fps();

	/* Call on every iteration */
	void Update(Uint32 elapsed);
};

World::World()
{
	elapsedUntil = 0;
	secsElapsed = 0;
}

std::string World::WindowTitle()
{
	return "[elapsed: " + std::to_string(secsElapsed) + "s] Main Window";
}

Uint32 World::Fps()
{
	return 60;
}

void World::Update(Uint32 elapsed)
{
	elapsedUntil += elapsed;
	if (elapsedUntil >= 1000)
	{
		secsElapsed ++;
		elapsedUntil = 0;
		// Schedule window title change on next iteration
		Schedule(ScheduleFlags::None().WindowTitle());
	}
}

int main()
{
	World world;
	loop(world);
	return 0;
}
```

Compile using `g++ -o example -lSDL2 -lSDLProtoEngine example.cc`