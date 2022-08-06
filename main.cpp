#include "menu_class.hpp"
#include "desktop.hpp"


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#define IN_DESKTOP(X)  my_desktop.GET_DESKTOP_PATH()+"\\"+X 
#define var_dict(X) {{"filename",IN_DESKTOP(X)}}
#define DEFAULT_EXTENSION "default"

#include <iostream>
#include <map>
#include <vector>
using namespace std;


int main(int argc, char const* argv[])
{
	extension_menu_dictionary menuitems = menu_utils::get_popups_from_file("src/menu_items.xml", "src/vars.xml");

	vector<filename> file_list = my_desktop.get_filenames();
	vector<foldername> folder_list = my_desktop.get_foldernames();
	size_t i;
	for (i = 0; i < file_list.size(); i++)
	{
		filename cur_file = file_list[i];
		extension ext = menu_utils::extension_of(cur_file);

		if (menuitems.find(ext) == menuitems.end())
			ext = DEFAULT_EXTENSION;
		p_menu* menu = new p_menu(menuitems[ext]);
		my_desktop.iconset[i].set_icon(
			&my_desktop,
			menu,
			cur_file,
			my_desktop.GET_DESKTOP_PATH(),
			ext
		);
		std::vector<p_item>& items = my_desktop.iconset[i].get_menu()->menu_items();

		for (p_item& p : items)
			p.set_cmd(menu_utils::mustache_parse(p.get_cmd(), var_dict(cur_file)));
	}
	for (i = 0; i < folder_list.size(); i++)
	{
		foldername cur_folder = folder_list[i];
		p_menu* menu = new p_menu(menuitems[FOLDER_EXTENSION]);
		my_desktop.iconset[i].set_icon(
			&my_desktop,
			menu,
			cur_folder,
			my_desktop.GET_DESKTOP_PATH()
		);
		std::vector<p_item>& items = my_desktop.iconset[i].get_menu()->menu_items();
		for (p_item& p : items)
			p.set_cmd(menu_utils::mustache_parse(p.get_cmd(), var_dict(cur_folder)));
	}

	my_desktop.render();
	return 0;
}