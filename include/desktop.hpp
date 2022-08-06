#include "desktop.h"
#include "controls.hpp"

#include <algorithm>
#include <regex>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define WHOLE_SCREEN irr::core::rect<irr::s32>(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)

const foldername FOLDER_RES_FOLDER = "./res/folder/";
const foldername FILE_RES_FOLDER = "./res/file/";
const char* BG_MESH = "D:/irrlicht-projects/toplevel_source/VS/toplevel/res/plane.obj";


std::string exec(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	return result;
}

desktop::desktop()
{

	this->device =
		irr::createDevice(
			irr::video::EDT_OPENGL,
			irr::core::dimension2d<irr::u32>(SCREEN_WIDTH, SCREEN_HEIGHT),
			16,
			false, false, false,
			&this->receiver
		);

	if (!device)
		return;

	this->device->setWindowCaption(L"Parallasyx");

	this->driver = device->getVideoDriver();
	this->smgr = device->getSceneManager();
	this->guienv = device->getGUIEnvironment();

	cmdout user_name = exec("whoami");
	user_name.erase(user_name.end() - 1);
	std::string c("\\");
	auto f_it = std::find_end(user_name.begin(), user_name.end(), c.begin(), c.end());
	user_name.erase(user_name.begin(), f_it + 1);
	user_name.shrink_to_fit();

	cmdout desktop_path = "C:\\Users\\" + user_name + "\\Desktop";
	this->DESKTOP_PATH = desktop_path;

	cmdout desktop_text_contents = exec(c_string("dir /b " + desktop_path));
	cmdout desktop_text_contents_f = exec(c_string("dir /b /AD " + desktop_path));

	std::stringstream sstr(desktop_text_contents_f); // Getting filenames line by line stringstream technique
	std::string token;

	while (std::getline(sstr, token, '\n'))
	{
		this->foldernames.push_back(token);
		this->iconset.push_back(icon());
	}

	std::stringstream t_sstr(desktop_text_contents);
	while (std::getline(t_sstr, token, '\n'))
	{
		if (find(foldernames.begin(), foldernames.end(), token) == foldernames.end())
		{
			this->filenames.push_back(token);
			this->iconset.push_back(icon());
		}
	}
}

desktop::~desktop() {}

std::vector<filename> desktop::get_filenames() const { return filenames; }
std::vector<foldername> desktop::get_foldernames() const { return foldernames; }
foldername desktop::GET_DESKTOP_PATH() const { return DESKTOP_PATH; }

void desktop::render()
{
	irr::scene::IAnimatedMesh* mesh = smgr->getMesh(BG_MESH);
	if (!mesh)
	{
		device->drop();
		return;
	}
	irr::scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh, NULL, 1, irr::core::vector3df(0, 0, 0));

	while (device->run() and driver)
	{
		driver->beginScene(true, true, irr::video::SColor(0, 0, 0, 0));

		driver->getMaterial2D().TextureLayer[0].BilinearFilter = true;
		driver->getMaterial2D().AntiAliasing = irr::video::EAAM_ALPHA_TO_COVERAGE;

		irr::scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, irr::core::vector3df(0, 0, -4), irr::core::vector3df(0, 0, 0));

		if (node)
		{
			node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
			node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
			//node->setMD2Animation(irr::scene::EMAT_STAND);
			node->setMaterialTexture(0, driver->getTexture("./res/wallpaper.png"));
		}

		irr::core::vector3df nodePosition = node->getPosition();
		irr::core::position2di m_pos = receiver.GetMouseState().Position;

		// Create a ray through the mouse cursor
		irr::core::line3df ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(m_pos, camera);


		// And intersect the ray with a plane around the node facing towards the camera.
		irr::core::plane3df plane(nodePosition - irr::core::vector3df(0, 1, 0), irr::core::vector3df(0, 1, 1));
		irr::core::vector3df mousePosition;
		if (plane.getIntersectionWithLine(ray.start, ray.getVector(), mousePosition))
		{
			irr::core::vector3df result;
			irr::core::vector3df rot;

			result = nodePosition - mousePosition;
			rot = result.getHorizontalAngle();

			//node->setRotation(rot);
			node->setRotation(rot.rotationToDirection(mousePosition) / 5);
		}

		smgr->drawAll();
		guienv->drawAll();

		for (icon& each_icon : iconset)
		{
			if (each_icon.is_set())
			{
				each_icon.render_icon();
				each_icon.update_with(receiver.MouseState.Position);

				if (not menu_is_active)
				{
					if ((each_icon.hovered() and receiver.MouseState.RightButtonDown) or (each_icon.hovered() and menu_is_active))
					{
						if (each_icon.get_menu()->get_items() not_eq 0)
						{
							active_menu_icon = &each_icon;
							menu_is_active = true;
						}
					}
				}
				auto ws(std::to_wstring(receiver.MouseState.Position.X));
			}
		}

		if (active_menu_icon != nullptr and menu_is_active)
		{
			for (auto& each_item : active_menu_icon->get_menu()->menu_items())
			{
				each_item.update_with(receiver.MouseState.Position);
				if (receiver.MouseState.LeftButtonDown and each_item.hovered())
					system(each_item.cmd());
			}

			active_menu_icon->get_menu()->render_menu(driver, active_menu_icon->get_icon_coords(), device->getGUIEnvironment()->getBuiltInFont());

			if (not active_menu_icon->hovered() and (receiver.MouseState.LeftButtonDown or receiver.MouseState.RightButtonDown))
			{
				menu_is_active = false;
				active_menu_icon = nullptr;
			}
		}

		irr::core::position2di MPos = receiver.GetMouseState().Position;
		std::stringstream ss;
		std::string s;
		ss << MPos.X << ":" << MPos.Y;
		ss >> s;
		std::wstring ws(s.begin(), s.end());

		reset_coords();
		driver->endScene();
	}
	device->drop();
	return;
}


irr::core::position2d<irr::s32> icon::get_icon_coords() const { return render_coords; }
bool icon::hovered() const { return hover; }
bool icon::is_set() const { return _is_set; }
p_menu* icon::get_menu() const { return menu; };

void icon::update_with(irr::core::position2di mouse_pos)
{
	auto& lr = hover_area.LowerRightCorner, & ul = hover_area.UpperLeftCorner;
	if ((mouse_pos.X <  lr.X and mouse_pos.X > ul.X) and (mouse_pos.Y <  lr.Y and mouse_pos.Y > ul.Y))
		this->hover = true;
	else
		this->hover = false;
}

void icon::set_icon(desktop* parent,
	p_menu* p,
	filename title_2,
	foldername f = ".",
	extension ext = FOLDER_EXTENSION)
{
	f_title = title_2;
	title = std::wstring(title_2.begin(), title_2.end());
	menu = p;
	f_name = f;
	_is_set = true;
	parent_desktop = parent;
	icon_file = appropriate_icon(ext);
}

void icon::add_hover_region(irr::core::rect<irr::s32> _hover_area) { hover_area = _hover_area; }

void icon::render_icon()
{
	irr::gui::IGUIFont* font = parent_desktop->device->getGUIEnvironment()->getBuiltInFont();
	irr::video::ITexture* ico = parent_desktop->driver->getTexture(get_icon_file().c_str());
	irr::s32& x = parent_desktop->x, & y = parent_desktop->y, & items = parent_desktop->items, & per_row = parent_desktop->per_row;

	const irr::u32& WIDTH = ico->getSize().Width, & HEIGHT = ico->getSize().Height;

	irr::s32 C = 255 - 50 * (int)((parent_desktop->active_menu_icon == this) or hover and not parent_desktop->receiver.MouseState.LeftButtonDown);

	irr::core::rect<irr::s32> cliprect = irr::core::rect<irr::s32>(x, y, x + WIDTH, y + HEIGHT), * _cliprect = &cliprect;

	parent_desktop->driver->draw2DImage(
		ico,
		irr::core::position2d(x, y),
		irr::core::rect<irr::s32>(0, 0, WIDTH, HEIGHT),
		_cliprect /*WHOLE_SCREEN*//*<-Next edit here*/,
		irr::video::SColor(255, C, C, C),
		true);

	font->draw(to_maxlength(title).c_str(), irr::core::rect<irr::s32>(irr::core::position2d<irr::s32>(x + WIDTH * 0.1, y + HEIGHT * 0.95), irr::core::dimension2d<irr::s32>(10, 100)), irr::video::SColor(255, 255, 255, 255));
	add_hover_region(irr::core::rect<irr::s32>(x, y, x + WIDTH, y + HEIGHT));

	render_coords.set(x + HEIGHT / 2, y + WIDTH / 2);

	x += WIDTH * 1.2;
	items++;
	if (items % per_row == 0)
	{
		x = 10;
		y += HEIGHT * 1.2;
	}

}

void desktop::reset_coords()
{
	x = 10;
	y = 0;
	items = 0;
	per_row = desktop::per_row;
}

std::wstring to_maxlength(std::wstring& t, int size)
{
	if (t.size() > size)
	{
		std::string _k(t.begin(), t.begin() + 10);
		_k += "...";
		return std::wstring(_k.begin(), _k.end());
	}
	return t;
}


filename icon::appropriate_icon(extension ext)
{
	if (ext not_eq FOLDER_EXTENSION)
	{
		std::stringstream sstr(file_icon_files); // Getting individual icon_files line by line stringstream technique
		std::string token;

		while (std::getline(sstr, token, '\n'))
		{
			std::string t2 = token;
			auto f_it2 = t2.find_last_of(".");
			t2.erase(t2.begin() + f_it2, t2.end());
			t2.shrink_to_fit();

			std::regex re(c_string(t2));
			std::cmatch m;

			if (std::regex_search(c_string(f_title), m, re))
			{
				return FILE_RES_FOLDER + token;
			}
		}
		return  FILE_RES_FOLDER + "File.png";
	}

	std::stringstream sstr(folder_icon_files); // Getting individual icon_files line by line stringstream technique
	std::string token;

	while (std::getline(sstr, token, '\n'))
	{
		std::string t = token;
		auto f_it = t.find_last_of(".");
		t.erase(t.begin() + f_it, t.end());
		t.shrink_to_fit();

		std::regex re(c_string(t));
		std::cmatch m;

		if (std::regex_search(c_string(f_title), m, re))
		{
			return FOLDER_RES_FOLDER + token;
		}
	}
	return  FOLDER_RES_FOLDER + "Folder.png";
}

filename icon::get_icon_file() const { return icon_file; }
filename icon::get_abs_filename() const { return f_name + f_title; };

icon::icon() {}
icon::~icon() {}



