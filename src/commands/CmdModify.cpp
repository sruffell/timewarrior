////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdModify (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  auto filter = getFilter (cli);
  std::vector <int> ids = cli.getIds ();
  std::vector <std::string> words = cli.getWords ();
  enum { MODIFY_START, MODIFY_STOP } op {MODIFY_START};

  if (words.empty())
    throw std::string("Must specify start|stop command to modify. See 'timew help modify'.");

  if (words.at(0) == "start")
    op = MODIFY_START;
  else if (words.at(0) == "stop")
    op = MODIFY_STOP;
  else 
    throw format ("'{1}' is an invalid argument for modify. See 'timew help modify'.", words.at(0));

  if (ids.empty ())
    throw std::string ("ID must be specified. See 'timew help modify'.");

  if (ids.size () > 1)
    throw std::string ("Only one ID may be specified. See 'timew help modify'.");

  Interval empty_filter = Interval();
  auto tracked = getTracked (database, rules, empty_filter);

  int id = *ids.begin();
  if (id > static_cast <int> (tracked.size ()))
    throw format ("ID '@{1}' does not correspond to any tracking.", id);

  Interval interval = tracked.at (tracked.size () - id);
  if (interval.range.is_open ())
    throw format ("Cannot modify open interval @{1}.", id);

  if (filter.range.start.toEpoch () == 0)
    throw std::string ("No updated time specified. See 'timew help modify'.");

  if (op == MODIFY_STOP)
    interval.range.end = filter.range.start;
  else if (op == MODIFY_START)
    interval.range.start = filter.range.start;
  else
    assert(!"op is invalid");

  if (interval.range.start > interval.range.end)
    throw format ("Cannot modify interval @{1} where start is after end.", id);

  database.deleteInterval (tracked[tracked.size () - id]);
  validate (cli, rules, database, interval);
  database.addInterval (interval);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
