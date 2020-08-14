////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020, Thomas Lauf, Shaun Ruffell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <commands.h>
#include <timew.h>
#include <JSON.h>
#include <IntervalFactory.h>

std::string read_input ()
{
  std::string content;
  std::string line;

  while (std::getline (std::cin, line))
  {
    content += line;
  }

  return content;
}

////////////////////////////////////////////////////////////////////////////////
int CmdImport (const CLI& cli, Rules& rules, Database& database)
{
  Journal& journal = database.journal ();
  const bool verbose = rules.getBoolean ("verbose");

  std::unique_ptr <json::array> json (dynamic_cast <json::array *>(json::parse (read_input ())));

  journal.startTransaction ();
  for (const auto& value : json->_data)
  {
    json::object* object = dynamic_cast <json::object *> (value);

    Interval interval = IntervalFactory::fromJson (*object);
    validate (cli, rules, database, interval);
    database.addInterval (interval, verbose);

    if (verbose)
    {
      std::cout << intervalSummarize (database, rules, interval);
    }
  }
  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
