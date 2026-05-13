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

#include <algorithm>
#include <cctype>
#include <stack>
#include <string>
#include <string_view>

#include "helpers/xgui.h"

// ---------------------------------------------------------------------------
// Color name -> terminal color index
// Matches the color table from the original re2c parser exactly.
// ---------------------------------------------------------------------------
static int html_color_index(std::string_view name)
{
    std::string lower(name);
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (lower == "black")                                return  0;
    if (lower == "navy")                                 return  1;
    if (lower == "green")                                return  2;
    if (lower == "cyan"      || lower == "teal")         return  3;
    if (lower == "red")                                  return  4;
    if (lower == "magenta"   || lower == "purple")       return  5;
    if (lower == "brown"     || lower == "maroon")       return  6;
    if (lower == "lightgray" || lower == "silver")       return  7;
    if (lower == "darkgray"  || lower == "gray")         return  8;
    if (lower == "lightblue" || lower == "blue")         return  9;
    if (lower == "lightgreen"|| lower == "lime")         return 10;
    if (lower == "lightcyan" || lower == "aqua")         return 11;
    if (lower == "lightred")                             return 12;
    if (lower == "lightmagenta" || lower == "fuchsia")   return 13;
    if (lower == "yellow")                               return 14;
    if (lower == "white")                                return 15;

    return -1;
}

// ---------------------------------------------------------------------------
// Case-insensitive string equality
// ---------------------------------------------------------------------------
static bool iequal(std::string_view a, std::string_view b)
{
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i])))
            return false;
    return true;
}

// ---------------------------------------------------------------------------
// Skip everything up to and including 'close_tag' (case-sensitive).
// ---------------------------------------------------------------------------
static void skip_block(std::string_view src, std::size_t& pos,
                       std::string_view close_tag)
{
    const auto found = src.find(close_tag, pos);
    pos = (found == std::string_view::npos) ? src.size() : found + close_tag.size();
}

// ---------------------------------------------------------------------------
// Parse "color=NAME" inside a tag content string.
// Returns color index or -1.
// ---------------------------------------------------------------------------
static int parse_font_color(std::string_view tag_content)
{
    const auto color_pos = tag_content.find("color=");
    if (color_pos == std::string_view::npos) return -1;

    std::size_t name_start = color_pos + 6;
    bool quoted = false;

    if (name_start < tag_content.size() &&
        (tag_content[name_start] == '"' || tag_content[name_start] == '\'')) {
        quoted = true;
        ++name_start;
    }

    std::size_t name_end = name_start;
    while (name_end < tag_content.size()) {
        const char c = tag_content[name_end];
        if (quoted ? (c == '"' || c == '\'') : (c == ' ' || c == '\t' || c == '>'))
            break;
        ++name_end;
    }

    return html_color_index(tag_content.substr(name_start, name_end - name_start));
}

// ---------------------------------------------------------------------------
// Parse href="FILENAME" inside a tag content string.
// Returns the filename or empty string.
// ---------------------------------------------------------------------------
static std::string parse_href(std::string_view tag_content)
{
    const auto href_pos = tag_content.find("href=");
    if (href_pos == std::string_view::npos) return {};

    std::size_t val_start = href_pos + 5;
    bool quoted = false;

    if (val_start < tag_content.size() &&
        (tag_content[val_start] == '"' || tag_content[val_start] == '\'')) {
        quoted = true;
        ++val_start;
    }

    std::size_t val_end = val_start;
    while (val_end < tag_content.size()) {
        const char c = tag_content[val_end];
        if (quoted ? (c == '"' || c == '\'') : (c == ' ' || c == '\t' || c == '>'))
            break;
        ++val_end;
    }

    return std::string(tag_content.substr(val_start, val_end - val_start));
}

// ---------------------------------------------------------------------------
// XGuiList::AddHtmlText
//
// Supported constructs:
//   <font color=NAME>   push color, emit ESC+index
//   </font>             pop color, emit ESC+previous (lightgray fallback)
//   <a href="FILE">     flush current item, begin selectable ref
//   </a>                flush selectable item, end ref
//   <br>                emit '\n'
//   </h1> … </h6>       emit "\n\n"  (opening heading tags are ignored)
//   <head>…</head>      entire block skipped
//   <table>…</table>    entire block skipped
//   <form>…</form>      entire block skipped
//   <!--…-->            comment skipped
//   \n \r               skipped (whitespace normalized by SetWidth later)
//   other tags          silently ignored
//   plain text          appended verbatim
// ---------------------------------------------------------------------------
void XGuiList::AddHtmlText(std::string_view text)
{
    bool             is_ref = false;
    std::stack<char> colors;
    std::string      ref_name;
    std::string      out;

    // Flush the current accumulator as a new list item.
    const auto flush = [&]() {
        if (!out.empty()) {
            AddItem(new XGuiItem_Text(out.c_str(), is_ref ? 1 : 0));
            out.clear();
        }
    };

    // Prepend the active color escape when starting a new output segment.
    const auto prepend_color = [&]() {
        if (out.empty() && !colors.empty()) {
            out += static_cast<char>(0x1F);
            out += static_cast<char>(colors.top());
        }
    };

    std::size_t pos = 0;

    while (pos < text.size()) {
        const char ch = text[pos];

        // End of input
        if (ch == '\0') {
            flush();
            return;
        }

        // Newline / carriage-return: skip
        if (ch == '\n' || ch == '\r') {
            ++pos;
            continue;
        }

        // Tag
        if (ch == '<') {
            ++pos; // consume '<'

            const std::size_t tag_start = pos;
            while (pos < text.size() && text[pos] != '>' && text[pos] != '\0')
                ++pos;

            const std::string_view tag_content(text.data() + tag_start, pos - tag_start);
            if (pos < text.size() && text[pos] == '>') ++pos; // consume '>'

            // Extract the tag name (first whitespace-delimited word).
            const std::size_t word_end =
                tag_content.find_first_of(" \t\r\n");
            const std::string_view tag_name =
                tag_content.substr(0, word_end == std::string_view::npos
                                          ? tag_content.size()
                                          : word_end);

            // Block tags — skip everything up to and including the closing tag
            if (iequal(tag_name, "head")) {
                skip_block(text, pos, "</head>");
                continue;
            }
            if (iequal(tag_name, "table")) {
                skip_block(text, pos, "</table>");
                continue;
            }
            if (iequal(tag_name, "form")) {
                skip_block(text, pos, "</form>");
                continue;
            }

            // HTML comment: <!-- … -->
            if (tag_content.substr(0, 3) == "!--") {
                const auto comment_end = text.find("-->", tag_start);
                pos = (comment_end == std::string_view::npos)
                    ? text.size()
                    : comment_end + 3;
                continue;
            }

            // <br>
            if (iequal(tag_name, "br")) {
                prepend_color();
                out += '\n';
                continue;
            }

            // </h1> … </h6>  (closing heading tags only; openings are ignored)
            if (tag_name.size() == 2 &&
                tag_name[0] == '/' &&
                std::tolower(static_cast<unsigned char>(tag_name[1])) == 'h') {
                prepend_color();
                out += "\n\n";
                continue;
            }
            if (tag_name.size() == 3 &&
                tag_name[0] == '/' &&
                std::tolower(static_cast<unsigned char>(tag_name[1])) == 'h' &&
                std::isdigit(static_cast<unsigned char>(tag_name[2]))) {
                prepend_color();
                out += "\n\n";
                continue;
            }

            // <font color=…>
            if (iequal(tag_name, "font")) {
                const int color = parse_font_color(tag_content);
                if (color != -1) {
                    colors.push(static_cast<char>(color));
                    out += static_cast<char>(0x1F);
                    out += static_cast<char>(color);
                } else {
                    // Unknown font tag — emit rawq
                    out += '<';
                    out += tag_content;
                    out += '>';
                }
                continue;
            }

            // </font>
            if (iequal(tag_name, "/font")) {
                if (!colors.empty()) {
                    colors.pop();
                    out += static_cast<char>(0x1F);
                    out += colors.empty()
                        ? static_cast<char>(7)   // lightgray fallback
                        : static_cast<char>(colors.top());
                }
                continue;
            }

            // <a href="…">
            if (iequal(tag_name, "a")) {
                const std::string href = parse_href(tag_content);
                if (!href.empty()) {
                    flush();
                    ref_name = href;
                    is_ref   = true;
                } else {
                    out += '<';
                    out += tag_content;
                    out += '>';
                }
                continue;
            }

            // </a>
            if (iequal(tag_name, "/a")) {
                flush();
                is_ref = false;
                ref_name.clear();
                continue;
            }

            // All other tags — silently ignore
            continue;
        }

        // Plain text character
        prepend_color();
        out += ch;
        ++pos;
    }

    flush();
}
