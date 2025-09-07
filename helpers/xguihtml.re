/*
This file is part of "Avanor, the Land of Mystery" roguelike game
Home page: http://www.avanor.com/
Copyright (C) 2000-2003 Vadim Gaidukevich

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <string>
#include <stack>

#include "helpers/xgui.h"

#define	YYCTYPE		char
#define	YYCURSOR	cursor
#define	YYLIMIT		cursor
#define	YYMARKER	marker
#define	YYFILL(n)

/*!re2c
	ws             = [ \t\n];
	HtmlSkip       = [\n\r]|
	                 "<head>"[\001-\377]*"</head>"|
	                 "<table>"[\001-\377]*"</table>"|
	                 "<form>"[\001-\377]*"</form>"|
	                 "<!--"[\001-\377]*"-->";
	HtmlColorOpen  = "<font color=";
	HtmlColorText  = [a-z]+;
	HtmlColorClose = ([\001-\377]\[>])*">";
	HtmlRefOpen    = "<a href=\"";
	HtmlRefText    = [0-9a-zA-Z_.]+;
	HtmlRefClose   = "\">";
	HtmlBr         = "<br>";
*/

static int html_decode_color(char * text)
{
	int color = -1;
	YYCTYPE * cursor = text;
	YYCTYPE * marker = text;
color_open:;
/*!re2c               
	HtmlColorOpen            { goto color_text; }
	[\000-\377]              { return -1; }
*/
color_text:;
/*!re2c
	"black"                  { color = 0; goto color_close; }
	"navy"                   { color = 1; goto color_close; }
	"green"                  { color = 2; goto color_close; }
	"cyan"|"teal"            { color = 3; goto color_close; }
	"red"                    { color = 4; goto color_close; }
	"magenta"|"purple"       { color = 5; goto color_close; }
	"brown"|"maroon"         { color = 6; goto color_close; }
	"lightgray"|"silver"     { color = 7; goto color_close; }
	"darkgray"|"gray"        { color = 8; goto color_close; }
	"lightblue"|"blue"       { color = 9; goto color_close; }
	"lightgreen"|"lime"      { color = 10; goto color_close; }
	"lightcyan"|"aqua"       { color = 11; goto color_close; }
	"lightred"               { color = 12; goto color_close; }
	"lightmagenta"|"fuchsia" { color = 13; goto color_close; }
	"yellow"                 { color = 14; goto color_close; }
	"white"                  { color = 15; goto color_close; }
	[\000-\377]              { return -1; }
*/
color_close:;
/*!re2c
	HtmlColorClose           { return color; }
	[\000-\377]              { return -1; }
*/
	return -1;
}

static std::string html_decode_ref(char * text)
{
	std::string ref;
	YYCTYPE * cursor = text;
	YYCTYPE * marker = text;
ref_open:;
/*!re2c
	HtmlRefOpen      { goto ref_text; }
	[\000-\377]      { return ""; }
*/
ref_text:;
	YYCTYPE * token = cursor;
/*!re2c
	HtmlRefText      { ref = std::string(token, cursor - token); goto ref_close; }
	[\000-\377]      { return ""; }
*/
ref_close:;
/*!re2c
	HtmlRefClose     { return ref; }
	[\000-\377]      { return ""; }
*/
	return "";
}

#define NEXT_TOKEN() { goto next; }

void XGuiList::AddHtmlText(char * text)
{
	bool             is_ref = false;
	std::stack<char> colors;
	std::string      ref_name;
	std::string      out;

	YYCTYPE * cursor = text;
	YYCTYPE * marker = text;
next:
	YYCTYPE * token = cursor;
/*!re2c
	HtmlSkip
	{ 
		NEXT_TOKEN(); 
	}

	"<font "([\001-\377]\[>])*">"
	{
		int color = html_decode_color(token);
		if (color != -1) 
		{
			colors.push(color); 
			out += (char)0x1F; 
			out += (char)color; 
		}
		else
		{
			out += std::string(token, cursor - token);
		}
		NEXT_TOKEN();
	}

	"</font>"             
	{
		if (!colors.empty())
		{
			colors.pop(); 
			out += (char)0x1F; 
			if (!colors.empty())
				out += (char)colors.top();
			else
				out += 7; // lightgray
		}
		NEXT_TOKEN(); 
	}

	"<a "([\001-\377]\[>])*">"
	{ 
		ref_name = html_decode_ref(token);
		if (!ref_name.empty())
		{
			if (!out.empty())
			{
				AddItem(new XGuiItem_Text(out.c_str(), is_ref));
				out = ""; 
			}
			is_ref = true;
		}
		else
		{
			out += std::string(token, cursor - token);
		}
		NEXT_TOKEN();
	}

	"</a>" 
	{ 
		if (!out.empty())
		{
			AddItem(new XGuiItem_Text(out.c_str(), is_ref));
			out = ""; 
		}
		is_ref = false;
		ref_name = "";
		NEXT_TOKEN();
	}

	"<br>"
	{ 
		out += '\n'; 
		NEXT_TOKEN(); 
	}

	"</h"[123456]">"
	{
		out += "\n\n"; 
		NEXT_TOKEN(); 
	}

	"<"([\001-\377]\[>])*">"
	{ 
		NEXT_TOKEN(); 
	}

	[\001-\377]
	{
		if (out.empty() && !colors.empty())
		{
			out += (char)0x1F; 
			out += (char)colors.top();
		}
		out += *token;
		NEXT_TOKEN();
	}

	[\000]
	{
		if (!out.empty())
		{
			AddItem(new XGuiItem_Text(out.c_str(), is_ref));
			out = "";
		}
		return;
	}
*/
}
