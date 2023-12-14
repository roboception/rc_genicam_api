/*
 * This file is part of the rc_genicam_api package.
 *
 * Copyright (c) 2023 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "nodemap_edit.h"

#ifdef INCLUDE_CURSES

#include "nodemap_out.h"

#include <ncurses.h>
#include <sstream>
#include <vector>
#include <iomanip>

namespace rcg
{

namespace
{

void mvaddstr_eol(int row, int col, const char *str, bool reverse=false)
{
  int cols=getmaxx(stdscr);

  if (col < cols)
  {
    int n=strlen(str);

    if (col+n > cols)
    {
      n=cols-col;
    }

    if (reverse) attron(WA_REVERSE);
    mvaddnstr(row, col, str, n);
    if (reverse) attroff(WA_REVERSE);
  }
}

class NodeParam
{
  private:

    int level;
    int value_column;
    GenApi::INode *node;

  public:

    NodeParam(int _level, GenApi::INode *_node)
    {
      level=_level;
      node=_node;
    }

    int getMinValueColum();
    void setValueColumn(int column);
    int getValueColumn();

    std::string getValue(bool add_unit_range);
    std::string getAllowedCharacters();

    void printName(int row);
    void printValue(int row, bool reverse);
    void printTooltip(int row, bool reverse);

    int getOptions(std::vector<std::string> &option);

    bool isWritable();
    std::string setValue(const std::string &value);

    bool isExecutable();
    std::string execute();
};

int NodeParam::getMinValueColum()
{
  return 2*level+static_cast<int>(node->GetName().size())+1;
}

void NodeParam::setValueColumn(int column)
{
  value_column=column;
}

int NodeParam::getValueColumn()
{
  return value_column;
}

std::string NodeParam::getValue(bool add_unit_range)
{
  std::ostringstream out;

  switch (node->GetPrincipalInterfaceType())
  {
    case GenApi::intfIBoolean:
      {
        GenApi::IBoolean *p=dynamic_cast<GenApi::IBoolean *>(node);

        if (GenApi::IsReadable(p))
        {
          if (p->GetValue())
          {
            out << "True";
          }
          else
          {
            out << "False";
          }
        }
      }
      break;

    case GenApi::intfIInteger:
      {
        GenApi::IInteger *p=dynamic_cast<GenApi::IInteger *>(node);

        if (GenApi::IsReadable(p))
        {
          out << formatValue(p, p->GetValue());

          if (add_unit_range)
          {
            out << " " << p->GetUnit();

            if (GenApi::IsWritable(node))
            {
              out << " [" << formatValue(p, p->GetMin()) << ", " << formatValue(p, p->GetMax()) <<
                "]";
            }
          }
        }
      }
      break;

    case GenApi::intfIRegister:
      {
        GenApi::IRegister *p=dynamic_cast<GenApi::IRegister *>(node);

        if (GenApi::IsReadable(p))
        {
          int len=static_cast<int>(p->GetLength());

          len=std::min(len, (getmaxx(stdscr)-value_column)/2-1);
          len=std::min(len, 128);

          uint8_t buffer[128];
          p->Get(buffer, len);

          out << std::hex;
          for (int i=0; i<len && i<len; i++)
          {
            out << std::setfill('0') << std::setw(2) << static_cast<int>(buffer[i]);
          }
        }
      }
      break;

    case GenApi::intfIFloat:
      {
        GenApi::IFloat *p=dynamic_cast<GenApi::IFloat *>(node);

        if (GenApi::IsReadable(p))
        {
          out << p->GetValue();

          if (add_unit_range)
          {
            out << " " << p->GetUnit();

            if (GenApi::IsWritable(node))
            {
              out << " [" << p->GetMin() << ", " << p->GetMax() << "]";
            }
          }
        }
      }
      break;

    case GenApi::intfIString:
      {
        GenApi::IString *p=dynamic_cast<GenApi::IString *>(node);

        if (GenApi::IsReadable(p))
        {
          out << p->GetValue();
        }
      }
      break;

    case GenApi::intfIEnumeration:
      {
        GenApi::IEnumeration *p=dynamic_cast<GenApi::IEnumeration *>(node);

        if (p)
        {
          if (GenApi::IsReadable(p->GetAccessMode()) && p->GetCurrentEntry() != 0)
          {
            out << p->GetCurrentEntry()->GetSymbolic();
          }
        }
      }
      break;

    case GenApi::intfICommand:
      out << "(execute)" << std::endl;
      break;

    default:
      break;
  }

  return out.str();
}

std::string NodeParam::getAllowedCharacters()
{
  std::string ret;

  switch (node->GetPrincipalInterfaceType())
  {
    case GenApi::intfIBoolean:
      ret.push_back('0');
      ret.push_back('1');
      break;

    case GenApi::intfIFloat:
      ret.push_back('.');
      ret.push_back('e');
      // and same as integer

    case GenApi::intfIInteger:
      {
        ret.push_back('-');
        ret.push_back('0');
        ret.push_back('1');
        ret.push_back('2');
        ret.push_back('3');
        ret.push_back('4');
        ret.push_back('5');
        ret.push_back('6');
        ret.push_back('7');
        ret.push_back('8');
        ret.push_back('9');

        GenApi::IInteger *p=dynamic_cast<GenApi::IInteger *>(node);
        if (p)
        {
          switch (p->GetRepresentation())
          {
            case GenApi::IPV4Address:
              ret.push_back('.');
              break;

            case GenApi::MACAddress:
              ret.push_back(':');
              // and same as hex number

            case GenApi::HexNumber:
              ret.push_back('a');
              ret.push_back('b');
              ret.push_back('c');
              ret.push_back('d');
              ret.push_back('e');
              ret.push_back('f');
              break;

            default:
              break;
          }
        }
      }
      break;

    case GenApi::intfIRegister:
      ret.push_back('0');
      ret.push_back('1');
      ret.push_back('2');
      ret.push_back('3');
      ret.push_back('4');
      ret.push_back('5');
      ret.push_back('6');
      ret.push_back('7');
      ret.push_back('8');
      ret.push_back('9');
      ret.push_back('a');
      ret.push_back('b');
      ret.push_back('c');
      ret.push_back('d');
      ret.push_back('e');
      ret.push_back('f');
      break;

    case GenApi::intfIString:
    case GenApi::intfIEnumeration:
       // empty set means all printable characters
       break;

    default:
      break;
  }

  return ret;
}

void NodeParam::printName(int row)
{
  mvaddstr_eol(row, 2*level, node->GetName().c_str());
}

void NodeParam::printValue(int row, bool reverse)
{
  std::string value=getValue(true);

  if (value.size() > 0)
  {
    mvaddstr_eol(row, value_column, value.c_str(), reverse);
  }
}

void NodeParam::printTooltip(int row, bool reverse)
{
  mvaddstr_eol(row, 0, node->GetToolTip().c_str(), reverse);
}

int NodeParam::getOptions(std::vector<std::string> &option)
{
  int ret=0;

  switch (node->GetPrincipalInterfaceType())
  {
    case GenApi::intfIBoolean:
      {
        GenApi::IBoolean *p=dynamic_cast<GenApi::IBoolean *>(node);

        if (GenApi::IsReadable(p))
        {
          option.push_back(std::string("False"));
          option.push_back(std::string("True"));

          if (p->GetValue())
          {
            ret=1;
          }
        }
      }
      break;

    case GenApi::intfIEnumeration:
      {
        GenApi::IEnumeration *p=dynamic_cast<GenApi::IEnumeration *>(node);

        if (p && GenApi::IsReadable(p))
        {
          std::string value;
          if (p->GetCurrentEntry() != 0)
          {
            value=p->GetCurrentEntry()->GetSymbolic().c_str();
          }

          GenApi::StringList_t list;
          p->GetSymbolics(list);

          for (size_t i=0; i<list.size(); i++)
          {
            std::string opt=list[i].c_str();
            option.push_back(opt);

            if (opt == value)
            {
              ret=i;
            }
          }
        }
      }
      break;

    default:
      break;
  }

  return ret;
}

bool NodeParam::isWritable()
{
  return GenApi::IsWritable(node);
}

std::string NodeParam::setValue(const std::string &value)
{
  std::string ret;

  try
  {
    if (GenApi::IsWritable(node))
    {
      switch (node->GetPrincipalInterfaceType())
      {
        case GenApi::intfIBoolean:
          {
            GenApi::IBoolean *p=dynamic_cast<GenApi::IBoolean *>(node);

            std::string v=std::string(value);
            if (v == "true" || v == "True" || v == "TRUE")
            {
              p->SetValue(1);
            }
            else if (v == "false" || v == "False" || v == "FALSE")
            {
              p->SetValue(0);
            }
            else
            {
              p->SetValue(static_cast<bool>(std::stoi(v)));
            }
          }
          break;

        case GenApi::intfIInteger:
          {
            GenApi::IInteger *p=dynamic_cast<GenApi::IInteger *>(node);

            switch (p->GetRepresentation())
            {
              case GenApi::HexNumber:
                p->SetValue(std::stoll(std::string(value), 0, 16));
                break;

              case GenApi::IPV4Address:
                {
                  int64_t ip=0;

                  std::stringstream in(value);
                  std::string elem;

                  for (int i=0; i<4; i++)
                  {
                    getline(in, elem, '.');
                    ip=(ip<<8)|(stoi(elem)&0xff);
                  }

                  p->SetValue(ip);
                }
                break;

              case GenApi::MACAddress:
                {
                  int64_t mac=0;

                  std::stringstream in(value);
                  std::string elem;

                  for (int i=0; i<4; i++)
                  {
                    getline(in, elem, ':');
                    mac=(mac<<8)|(stoi(elem, 0, 16)&0xff);
                  }

                  p->SetValue(mac);
                }
                break;

              default:
                p->SetValue(std::stoll(std::string(value)));
                break;
            }
          }
          break;

        case GenApi::intfIRegister:
          {
            GenApi::IRegister *p=dynamic_cast<GenApi::IRegister *>(node);

            std::vector<uint8_t> buffer;
            for (size_t i=0; i<value.size()-1; i+=2)
            {
              buffer.push_back(stoi(value.substr(i, 2), 0, 16));
            }

            p->Set(buffer.data(), buffer.size());
          }
          break;

        case GenApi::intfIFloat:
          {
            GenApi::IFloat *p=dynamic_cast<GenApi::IFloat *>(node);
            p->SetValue(std::stof(std::string(value)));
          }
          break;

        case GenApi::intfIEnumeration:
          {
            GenApi::IEnumeration *p=dynamic_cast<GenApi::IEnumeration *>(node);
            GenApi::IEnumEntry *entry=0;

            try
            {
              entry=p->GetEntryByName(value.c_str());
            }
            catch (const GENICAM_NAMESPACE::GenericException &)
            { }

            if (entry != 0)
            {
              p->SetIntValue(entry->GetValue());
            }
          }
          break;

        case GenApi::intfIString:
          {
            GenApi::IString *p=dynamic_cast<GenApi::IString *>(node);
            p->SetValue(value.c_str());
          }
          break;

        default:
          break;
      }
    }
    else
    {
      ret="Internal error: Node is not writable";
    }
  }
  catch (const GENICAM_NAMESPACE::GenericException &ex)
  {
    ret=std::string("Error: ")+ex.what();
  }
  catch (...)
  {
    ret="Error: Unknown exception";
  }

  return ret;
}

bool NodeParam::isExecutable()
{
  return (node->GetPrincipalInterfaceType() == GenApi::intfICommand);
}

std::string NodeParam::execute()
{
  std::string ret;

  try
  {
    if (GenApi::IsWritable(node))
    {
      GenApi::ICommand *val=dynamic_cast<GenApi::ICommand *>(node);

      if (val != 0)
      {
        val->Execute();
      }
      else
      {
        ret="Internal error: Feature not a command";
      }
    }
    else
    {
      ret="Internal error: Node is not writable";
    }
  }
  catch (const GENICAM_NAMESPACE::GenericException &ex)
  {
    ret=std::string("Error: ")+ex.what();
  }
  catch (...)
  {
    ret="Error: Unknown exception";
  }

  return ret;
}

/*
  Adding the given node and all child nodes recursively to the given list.
*/

void addNodeToList(std::vector<NodeParam> &list, GenApi::INode *node, int level)
{
  if (node->GetAccessMode() != GenApi::NI)
  {
    list.push_back(NodeParam(level, node));

    if (node->GetPrincipalInterfaceType() == GenApi::intfICategory)
    {
      GenApi::ICategory *root=dynamic_cast<GenApi::ICategory *>(node);

      if (root != 0)
      {
        GenApi::FeatureList_t feature;
        root->GetFeatures(feature);

        level++;
        for (size_t i=0; i<feature.size(); i++)
        {
          addNodeToList(list, feature[i]->GetNode(), level);
        }
      }
    }
  }
}

/*
  Redraw one line.
*/

void redraw_line(int row, NodeParam &node_param, bool focus, bool clear_line=false)
{
  if (clear_line)
  {
    move(row, 0);
    clrtoeol();
  }

  attr_t attrs=0;
  if (node_param.isWritable()) attrs|=A_BOLD;
  if (focus) attrs|=WA_REVERSE;

  attron(attrs);
  node_param.printName(row);
  attroff(attrs);

  node_param.printValue(row, false);
}

/*
  Clear and redraw everything, starting with the given top row, which may be
  changed such that the focus row is always visible.
*/

void redraw(std::vector<NodeParam> &list, int &top_row, int focus_row, const char *message=0)
{
  int rows=getmaxy(stdscr);

  erase();

  if (rows > 1)
  {
    rows--; // last row is used for showing tool tips and errors

    // focus row must always be visible

    if (focus_row < top_row) top_row=focus_row;
    if (focus_row > top_row+rows-1) top_row=focus_row-rows+1;

    // print all visible rows

    for (int i=0; i<rows; i++)
    {
      if (top_row+i < static_cast<int>(list.size()))
      {
        redraw_line(i, list[top_row+i], focus_row == top_row+i);
      }
    }

    if (message && *message != '\0')
    {
      mvaddstr_eol(rows, 0, message, true);
    }
    else
    {
      list[focus_row].printTooltip(rows, true);
    }
  }
}

/*
  Editing loop for executing a command node.
*/

std::string editNodeExecute(std::vector<NodeParam> &list, int &top_row, int focus_row)
{
  std::string ret;

  bool edit=true;
  while (edit)
  {
    redraw_line(focus_row-top_row, list[focus_row], false, false);

    list[focus_row].printValue(focus_row-top_row, true);

    int c=getch();

    switch (c)
    {
      case KEY_RESIZE:
        redraw(list, top_row, focus_row);
        break;

      case KEY_ENTER:
      case '\n':
      case '\r':
        ret=list[focus_row].execute();
        if (ret.size() == 0)
        {
          ret="Command executed!";
        }
        edit=false;
        break;

      case 27:
      case 'q':
      case KEY_LEFT:
        edit=false;
        break;

      default:
        break;
    }
  }

  return ret;
}

/*
  Editing loop for cycling through options.
*/

std::string editNodeOption(std::vector<NodeParam> &list, int &top_row, int focus_row,
  const std::vector<std::string> &option, int current)
{
  std::string ret;

  redraw_line(focus_row-top_row, list[focus_row], false, false);

  bool edit=true;
  while (edit)
  {
    move(focus_row-top_row, list[focus_row].getValueColumn());
    clrtoeol();

    mvaddstr_eol(focus_row-top_row, list[focus_row].getValueColumn(), option[current].c_str(), true);

    int c=getch();

    switch (c)
    {
      case KEY_RESIZE:
        redraw(list, top_row, focus_row);
        redraw_line(focus_row-top_row, list[focus_row], false, false);
        break;

      case KEY_UP:
        if (current > 0)
        {
          current--;
        }
        break;

      case KEY_DOWN:
        if (current+1 < static_cast<int>(option.size()))
        {
          current++;
        }
        break;

      case KEY_ENTER:
      case '\n':
      case '\r':
        ret=list[focus_row].setValue(option[current]);
        edit=false;
        break;

      case 27:
      case 'q':
      case KEY_LEFT:
        edit=false;
        break;

      default:
        break;
    }
  }

  return ret;
}

/*
  Editing loop for entering a value as string.
*/

std::string editNodeString(std::vector<NodeParam> &list, int &top_row, int focus_row)
{
  std::string allowed=list[focus_row].getAllowedCharacters();
  std::string value=list[focus_row].getValue(false);
  std::string message="Current value: "+list[focus_row].getValue(true);
  std::string ret;

  // area for editing

  int row=focus_row-top_row;
  int cpos=static_cast<int>(value.size());
  int col0=list[focus_row].getValueColumn();
  int cn=getmaxx(stdscr)-col0;

  // unfocus

  redraw_line(row, list[focus_row], false, false);

  // show current value instead of tooltip

  move(getmaxy(stdscr)-1, 0);
  clrtoeol();

  mvaddstr_eol(getmaxy(stdscr)-1, 0, message.c_str(), true);

  // editing loop

  curs_set(1);

  bool edit=true;
  while (edit)
  {
    // redraw current value

    move(row, col0);
    clrtoeol();

    mvaddstr_eol(row, col0, value.c_str(), true);
    move(row, col0+cpos);

    // wait for next character

    int c=getch();

    switch (c)
    {
      case KEY_RESIZE:
        redraw(list, top_row, focus_row, message.c_str());
        redraw_line(row, list[focus_row], false, false);
        break;

      case KEY_LEFT:
        if (cpos > 0)
        {
          cpos--;
        }
        break;

      case KEY_RIGHT:
        if (cpos < static_cast<int>(value.size()) && cpos+1 < cn)
        {
          cpos++;
        }
        break;

      case KEY_BACKSPACE:
        if (cpos > 0)
        {
          cpos--;
          value.erase(cpos, 1);
        }
        break;

      case KEY_DC:
        if (cpos < static_cast<int>(value.size()))
        {
          value.erase(cpos, 1);
        }
        break;

      case KEY_ENTER:
      case '\n':
      case '\r':
        ret=list[focus_row].setValue(value);
        edit=false;
        break;

      case 27:
        edit=false;
        break;

      default:
        // allowed to increase value by one character?

        if (static_cast<int>(value.size()) < cn)
        {
          // is it a printable character?

          char cc=static_cast<char>(c);
          if (c < 256 && std::isprint(cc))
          {
            if (allowed.size() == 0 || allowed.find(cc) != std::string::npos)
            {
              value.insert(cpos, &cc, 1);
              cpos++;
            }
            else if (cc == 'q')
            {
              edit=false;
            }
          }
        }
        break;
    }
  }

  curs_set(0);

  return ret;
}

}

bool editNodemap(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char root[])
{
  // get list of all nodes

  std::vector<NodeParam> list;

  {
    GenApi::INode *node=nodemap->_GetNode(root);

    if (node)
    {
      addNodeToList(list, node, 0);
    }
    else
    {
      return false;
    }

    if (list.size() > 0 && std::string(root) == "Root")
    {
      list.erase(list.begin());
    }
  }

  // determine column at which to print values

  {
    int value_column=0;

    for (size_t i=0; i<list.size(); i++)
    {
      value_column=std::max(value_column, list[i].getMinValueColum());
    }

    for (size_t i=0; i<list.size(); i++)
    {
      list[i].setValueColumn(value_column);
    }
  }

  // initializing curses

  int top_row=0;
  int focus_row=0;

  initscr();

  try
  {
    curs_set(0);
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    nonl();

    redraw(list, top_row, focus_row);

    // enter navigation and editing loop

    bool run=true;
    while (run)
    {
      move(0, 0);
      int c=getch();

      switch (c)
      {
        case KEY_RESIZE:
          redraw(list, top_row, focus_row);
          break;

        case KEY_PPAGE:
          if (focus_row > 0)
          {
            int rows=getmaxy(stdscr);

            if (rows > 1)
            {
              rows--;
              top_row=std::max(0, top_row-rows);
              focus_row=std::max(0, focus_row-rows);
            }

            redraw(list, top_row, focus_row);
          }
          break;

        case KEY_NPAGE:
          if (focus_row+1 < static_cast<int>(list.size()))
          {
            int rows=getmaxy(stdscr);

            if (rows > 1)
            {
              rows--;
              top_row=std::min(top_row+rows, std::max(0, static_cast<int>(list.size())-rows));
              focus_row=std::min(focus_row+rows, static_cast<int>(list.size())-1);
            }

            redraw(list, top_row, focus_row);
          }
          break;

        case KEY_UP:
          if (focus_row > 0)
          {
            focus_row--;
            redraw(list, top_row, focus_row);
          }
          break;

        case KEY_DOWN:
          if (focus_row+1 < static_cast<int>(list.size()))
          {
            focus_row++;
            redraw(list, top_row, focus_row);
          }
          break;

        case KEY_RIGHT:
        case KEY_ENTER:
        case '\n':
        case '\r':
          if (list[focus_row].isWritable())
          {
            // edit value

            std::string message;

            if (list[focus_row].isExecutable())
            {
              message=editNodeExecute(list, top_row, focus_row);
            }
            else
            {
              std::vector<std::string> option;
              int current=list[focus_row].getOptions(option);

              if (option.size() > 0)
              {
                message=editNodeOption(list, top_row, focus_row, option, current);
              }
              else
              {
                message=editNodeString(list, top_row, focus_row);
              }
            }

            // redraw and show message

            redraw(list, top_row, focus_row, message.c_str());
          }
          break;

        case 27:
        case 'q':
          run=false;
          break;

        default:
          break;
      }
    }

    endwin();
  }
  catch (...)
  {
    endwin();
    throw;
  }

  return true;
}

}

#else

namespace rcg
{

bool editNodemap(const std::shared_ptr<GenApi::CNodeMapRef> &nodemap, const char root[])
{
  std::cerr << "Editing of nodemap is not implemented! Recompile with ncurses." << std::endl;
  return false;
}

}

#endif
