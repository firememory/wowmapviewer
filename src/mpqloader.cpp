#include "mpqloader.h"
#include "util.h"
#include "video.h"
#include "wowmapview.h"

MPQLoader::MPQLoader(std::string game_data_dir)
{	
	data_dir = game_data_dir;
	getLocale();

	// we'll just build a single MPQ list. the tick function
	// will be smart and load the first one as a base and
	// then patch everything else on top of it.

	const char* archiveNames[] = {
		"world.MPQ",
		"world2.MPQ", 
		"sound.MPQ",
		"art.MPQ",
		"expansion1.MPQ", 
		"expansion2.MPQ", 
		"expansion3.MPQ", 
		"$LOC/expansion1-locale-$LOC.MPQ",
		"$LOC/expansion2-locale-$LOC.MPQ",
		"$LOC/expansion3-locale-$LOC.MPQ", 
		"$LOC/locale-$LOC.MPQ",
	};
	for (size_t i=0; i<11; i++) {
		std::string mpq = game_data_dir;
		mpq.append(archiveNames[i]);
		//std::string mpq_name = archiveNames[i];
		replaceAll(mpq, "$LOC", locale);
		patch_list.push_back(mpq);
	}

	// add the patches to the end of the list
	findPatches(game_data_dir, "wow-update-base-*.MPQ");
	findPatches(game_data_dir.append(locale).append("/"), "wow-update-*.MPQ");

	num_patches = patch_list.size();
	num_loaded = 0;
}

MPQLoader::~MPQLoader()
{
	if (num_loaded > 0) root_mpq->close();
}

void MPQLoader::getLocale()
{
	char path[512];
	const char *locales[] = {"enUS", "enGB", "deDE", "frFR", "zhTW", "ruRU", "esES", "koKR", "zhCN"};

	for (size_t i=0; i<9; i++) {
		sprintf(path, "%s%s\\locale-%s.MPQ", data_dir.c_str(), locales[i], locales[i]);
		if (file_exists(path)) {
			locale = locales[i];
			break;
		}
	}
	locale = locales[0];
}

void MPQLoader::findPatches(std::string dir, const char* mask)
{
	std::string search_path = dir;
	search_path.append(mask);

	std::vector<std::string> matches;

#ifdef _WIN32
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	if ((hFind = FindFirstFile(search_path.c_str(), &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			//gLog("FOUND PATCH MPQ: %s%s\n", dir.c_str(), FindFileData.cFileName);
			matches.push_back(string(FindFileData.cFileName));
		} while(FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
#endif

	// these are for testing ordering...
	//matches.push_back("wow-update-base-11111.MPQ");
	//matches.push_back("wow-update-base-99999.MPQ");

	sort(matches.begin(), matches.end());

	for (std::vector<std::string>::iterator it = matches.begin(); it != matches.end(); ++it)
	{
		std::string mpq_path = dir;
		patch_list.push_back(mpq_path.append(*it));
	}
}

void MPQLoader::replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void MPQLoader::tick(float t, float dt)
{
	if (num_loaded == num_patches)
	{
		OpenDBs();
		Menu *m = new Menu();
		gStates.pop_back();
		gStates.push_back(m);
		return;
	}

	if (num_loaded == 0)
	{
		root_mpq = new MPQArchive(patch_list.at(0).c_str());
		num_loaded++;
		return;
	}

	root_mpq->applyPatch(patch_list.at(num_loaded).c_str());
	num_loaded++;
}

void MPQLoader::display(float t, float dt)
{
	video.clearScreen();
	glDisable(GL_FOG);
	video.set2D();
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	glColor4f(1,1,1,1);

	glEnable(GL_TEXTURE_2D);

	int basex = 200;
	int basey = 0;
	int tilesize = 12;


	char buffer[512];
	sprintf(buffer, "Loading MPQs (%d/%d)", num_loaded, num_patches);
	f32->shprint(video.xres/2 - f32->textwidth(buffer)/2, video.yres/2-16, buffer);
}

void MPQLoader::keypressed(SDL_KeyboardEvent *e)
{
}

void MPQLoader::mousemove(SDL_MouseMotionEvent *e)
{
}

void MPQLoader::mouseclick(SDL_MouseButtonEvent *e)
{
}
