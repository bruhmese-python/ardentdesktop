#include <regex>
#include <cctype>
#include <algorithm>
#include "menu_class.h"

#define p_item_img driver->getTexture("./res/item_image.png")
#define p_item_active_img driver->getTexture("./res/item_active_image.png")
#define p_header_img driver->getTexture("./res/header_image.png")
#define p_footer_img driver->getTexture("./res/footer_image.png")

p_menu::p_menu() {}
p_menu::~p_menu() {}

std::wstring to_maxlength(std::wstring& t, int size);

void p_menu::add_new_p_item(p_item p)
{
	this->p_menu_items.insert(p_menu_items.end(), p);

	n_items++;
}
void p_menu::render_menu(irr::video::IVideoDriver* driver, irr::core::position2d<irr::s32> m_pos, irr::gui::IGUIFont* font)
{
	size_t N = p_menu_items.size(), _n;
	irr::video::ITexture* render_image;
	render_image = p_header_img;
	p_item().render_item(driver, m_pos, render_image, font);
	m_pos.Y += render_image->getSize().Height;
	for (size_t i = 0; i < N; i++)
	{
		if (p_menu_items[i].hovered())
			render_image = p_item_active_img;
		else
			render_image = p_item_img;
		p_menu_items[i].render_item(driver, m_pos, render_image, font);
		m_pos.Y += render_image->getSize().Height;
	}
	render_image = p_footer_img;
	p_item().render_item(driver, m_pos, render_image, font);
}

int p_menu::get_items() const { return n_items; };
std::vector<p_item>& p_menu::menu_items() { return p_menu_items; }

p_menu& p_menu::operator=(const p_menu& D)
{
	n_items = D.get_items();
	p_menu_items = D.p_menu_items;
	/*header_image = D.header_image;
	footer_image = D.footer_image;*/
	return *this;
}

void p_item::update_with(irr::core::position2di mouse_pos)
{
	auto& lr = hover_area.LowerRightCorner, & ul = hover_area.UpperLeftCorner;
	if ((mouse_pos.X <  lr.X and mouse_pos.X > ul.X) and (mouse_pos.Y <  lr.Y and mouse_pos.Y > ul.Y))
		this->hover = true;
	else
		this->hover = false;
}

void p_item::set_cmd(arg cmd2) { second = cmd2; }
par p_item::get_cmd() { return second; }

p_item::p_item() {}
p_item::p_item(par parameter, arg argument) : first(parameter), second(argument) {}
p_item::~p_item() {}

const char* p_item::cmd() { return second.c_str(); }

void p_item::add_hover_region(irr::core::rect<irr::s32>& h_r) { hover_area = h_r; }

void p_item::render_item(irr::video::IVideoDriver* driver, irr::core::position2d<irr::s32>& r_pos, irr::video::ITexture* img, irr::gui::IGUIFont* font)
{
	const auto& WIDTH = img->getSize().Width, & HEIGHT = img->getSize().Height;

	irr::core::rect<irr::s32> cliprect = irr::core::rect<irr::s32>(r_pos,irr::core::dimension2d<irr::s32>(WIDTH,HEIGHT)), * _cliprect = &cliprect;
	driver->draw2DImage(
		img,
		r_pos,
		irr::core::rect<irr::s32>(0, 0, WIDTH, HEIGHT),
		_cliprect /*WHOLE_SCREEN*//*<-Next edit here*/,
		irr::video::SColor(255, 255, 255, 255),
		true);

	//driver->draw2DImage(img, r_pos, WHOLE_SCREEN, 0,
	//	irr::video::SColor(255, 255, 255, 255), true);

	irr::core::position2d<irr::s32> _r_pos(r_pos);
	_r_pos.X += 10;

	irr::core::rect<irr::s32> hover_region(r_pos, r_pos + irr::core::position2d<irr::s32>(WIDTH, HEIGHT));
	add_hover_region(hover_region);

	std::wstring ws = std::wstring(first.begin(), first.end());
	font->draw(to_maxlength(ws, 20).c_str(), irr::core::rect<irr::s32>(_r_pos, irr::core::dimension2d<irr::s32>(10, 100)), irr::video::SColor(255, 255, 255, 255));
}
bool p_item::hovered() const { return hover; }

extension_menu_dictionary menu_utils::get_popups_from_file(std::string filename, std::string template_variables_file)
{
	extension_menu_dictionary resultant_menus;

	rapidxml::file<> xmlFile(filename.c_str()); // Default template is char
	rapidxml::xml_document<> doc;               // character type defaults to char
	doc.parse<0>(xmlFile.data());               // 0 means default parse flags;

	for (rapidxml::xml_node<>* f_node = doc.first_node(); // Get node with fileformat as name
		f_node; f_node = f_node->next_sibling())
	{
		std::unordered_map<std::string, std::string> param_map;
		if (f_node->first_node())
		{
			for (rapidxml::xml_node<>* node = f_node->first_node();
				node; node = node->next_sibling())
			{
				param_map[node->name()] = f_node->first_node(std::string(node->name()).c_str())->value();
			}

			auto J = [template_variables_file](const std::string& str) { return menu_utils::mustache_parse(str, template_variables_file); };
			auto del_WS = [](std::string& str) {std::replace(str.begin(), str.end(), '_', ' '); };

			p_menu menu;
			for (const std::pair<std::string, std::string> p : param_map)
			{
				std::string first(J(p.first)), second(J(p.second));
				del_WS(first);
				menu.add_new_p_item(p_item(first, second));
			}
			resultant_menus[f_node->name()] = menu;

		}
	}
	return resultant_menus;
}
bool menu_utils::to_bool(std::string bool_str)
{
	std::transform(bool_str.begin(), bool_str.end(), bool_str.begin(), tolower);
	return (bool_str == "TRUE") ? true : false;
}
std::string menu_utils::mustache_parse(std::string text, std::unordered_map<std::string, std::string > addtnl_vars)
{
	for (const auto& [p, q] : addtnl_vars)
	{
		std::string r = "\\{\\{" + p + "\\}\\}";
		text = std::regex_replace(text, std::regex(r), q);
	}
	return text;
}
std::string menu_utils::mustache_parse(std::string text, std::string template_variables_file)
{
	rapidxml::file<> xmlFile(template_variables_file.c_str()); // Default template is char
	rapidxml::xml_document<> doc;                              // character type defaults to char
	doc.parse<0>(xmlFile.data());                              // 0 means default parse flags;
	for (rapidxml::xml_node<>* r_node = doc.first_node(); // Get node with fileformat as name
		r_node; r_node = r_node->next_sibling())
	{
		std::string r = "\\{\\{" + std::string(r_node->name()) + "\\}\\}";
		text = std::regex_replace(text, std::regex(r), r_node->value());
	}
	return text;
}

extension menu_utils::extension_of(filename f)
{
	std::string c = ".";
	auto f_it = std::find_end(f.begin(), f.end(), c.begin(), c.end());
	f.erase(f.begin(), f_it + 1);
	f.shrink_to_fit();
	return f;
}