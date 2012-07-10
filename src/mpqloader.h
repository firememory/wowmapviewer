#ifndef MPQLOADER_H
#define MPQLOADER_H

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <winerror.h>
#endif

#include <ctime>
#include <cstdlib>

#include "appstate.h"
#include "mpq.h"
#include "areadb.h"
#include "menu.h"

class MPQLoader :public AppState
{
	std::string data_dir;
	std::string locale;
	std::vector<std::string> patch_list;

	MPQArchive* root_mpq;

	int num_patches;
	int num_loaded;

	void findPatches(std::string dir, const char* mask);
	void getLocale();
	void replaceAll(std::string& str, const std::string& from, const std::string& to);

public:
	MPQLoader(std::string game_data_dir);
	~MPQLoader();

	void tick(float t, float dt);
	void display(float t, float dt);

	void keypressed(SDL_KeyboardEvent *e);
	void mousemove(SDL_MouseMotionEvent *e);
	void mouseclick(SDL_MouseButtonEvent *e);
};


#endif