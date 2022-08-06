#ifndef DESKTOP_H
#define DESKTOP_H

#pragma once

#include <stdlib.h>
#include <string>
#include<irrlicht.h>
#include "driverChoice.h"
#include<memory>
#include"controls.hpp"

#define c_string(x) const_cast<const char *>(std::string(x).c_str())
#define FOLDER_EXTENSION "folder"

std::string exec(const char*);

enum itemsof_p
{
	title,
	syscommand,
	icon_path,
	__is_submenu__,
	properties
};
std::string s_itemsof_p[] = {
	"title",
	"syscommand",
	"icon_path",
	"__is_submenu__",
	"properties" };

typedef std::string cmdout;
typedef std::string filename, foldername;

std::wstring to_maxlength(std::wstring& t, int size = 10);

class desktop;
class icon
{
private:
	static std::string file_icon_files, folder_icon_files;

	p_menu* menu;
	std::string f_title;
	foldername f_name;
	filename icon_file;
	bool _is_set = false;
	bool hover = false;
	static bool parent_referenced;
	desktop* parent_desktop;
	irr::core::rect<irr::s32> hover_area;
	irr::core::position2d<irr::s32> render_coords;

	filename appropriate_icon(extension ext);

public:
	std::wstring title;

	void set_icon(desktop*, p_menu*, filename, foldername, extension);
	void add_hover_region(irr::core::rect<irr::s32>);

	irr::core::position2d<irr::s32> get_icon_coords() const;
	filename get_icon_file() const;
	filename get_abs_filename() const;
	p_menu* get_menu() const;
	bool is_set() const;
	bool hovered() const;
	void update_with(irr::core::position2di);

	void render_icon();

	icon();
	icon(p_menu p,
		std::string title_2,
		foldername f);
	~icon();
};
std::string icon::file_icon_files = exec(c_string("dir /b \"res/file\""));
std::string icon::folder_icon_files = exec(c_string("dir /b \"res/folder\""));

bool icon::parent_referenced = false;

class desktop
{
	irr::s32 x = 10, y = 0, items = 0, per_row = 20;
	std::vector<filename> filenames;
	std::vector<foldername> foldernames;
	foldername DESKTOP_PATH;
	icon* active_menu_icon = nullptr;
	bool menu_is_active = false;

	MyEventReceiver receiver;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::scene::ISceneManager* smgr;
	irr::gui::IGUIEnvironment* guienv;

public:
	std::vector<icon> iconset;

	std::vector<filename> get_filenames() const;
	std::vector<foldername> get_foldernames() const;
	foldername GET_DESKTOP_PATH() const;

	void render();
	void reset_coords();

	desktop();
	~desktop();

	friend class icon;

private:
}my_desktop;

#endif