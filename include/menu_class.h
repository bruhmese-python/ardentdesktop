#ifndef MENU_CLASS_H
#define MENU_CLASS_H

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <irrlicht.h>
#include "driverChoice.h"
#include "controls.hpp"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define WHOLE_SCREEN irr::core::rect<irr::s32>(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)

typedef std::string extension, var;

typedef std::vector<std::size_t> args;
typedef std::vector<std::string> params;
typedef std::string filename, arg, par;

class p_item
{
private:
	std::string first = "", second = "";
	bool hover = false;
	bool is_disabled;
	irr::core::rect<irr::s32> hover_area;

public:
	filename bg_image;

	void render_item(irr::video::IVideoDriver*, irr::core::position2d<irr::s32>&, irr::video::ITexture*, irr::gui::IGUIFont*);
	bool hovered() const;
	void update_with(irr::core::position2di);
	void add_hover_region(irr::core::rect<irr::s32>&);

	void set_cmd(arg);
	par get_cmd();
	const char* cmd();

	p_item();
	p_item(par, arg);
	~p_item();
};

class p_menu
{
private:
	int n_items = 0;
	std::vector<p_item> p_menu_items;
	// int menu_ID;

public:
	void add_new_p_item(p_item);
	void render_menu(irr::video::IVideoDriver* driver, irr::core::position2d<irr::s32>, irr::gui::IGUIFont*);
	std::vector<p_item>& menu_items();

	int get_items() const;
	p_menu& operator=(const p_menu& D);
	p_menu();
	~p_menu();
};

typedef std::unordered_map<extension, p_menu> extension_menu_dictionary;
typedef std::unordered_map<var, p_menu> var_menu_dictionary;

namespace menu_utils
{
	bool to_bool(std::string);
	extension_menu_dictionary get_popups_from_file(std::string, std::string);
	std::string mustache_parse(std::string, std::string);
	std::string mustache_parse(std::string, std::unordered_map<std::string, std::string >);
	extension extension_of(filename);
}
#endif