#include <iostream>
#include <map>
#include <vector>
#include "menu_class.hpp"
#include "desktop.cpp"
using namespace std;
int main(int argc, char const *argv[])
{
	cout << "got filenames\n";
	extension_menu_dictionary menuitems = menu_utils::get_popups_from_file("src\\menu_items.xml", "src\\vars.xml");
	desktop my_desktop;

	vector<filename> file_list = my_desktop.get_filenames();
	for (size_t i = 0; i < file_list.size(); i++)
	{
		filename cur_file = file_list[i];
		cout << i << " " << cur_file << "\n";
		p_menu menu(menuitems[menu_utils::get_extension(cur_file)]);
		my_desktop.iconset[i].set_icon(menu,
									   cur_file,
									   my_desktop.GET_DESKTOP_PATH());
	}
	cout << "Set menus for all icons\n";

	for (icon ico : my_desktop.iconset)
	{
		ico.render_icon();
	}
	return 0;
}