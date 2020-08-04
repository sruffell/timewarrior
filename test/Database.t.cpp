////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020, Shaun Ruffell, Thomas Lauf.
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

#include <algorithm>
#include <cassert>
#include <test.h>
#include <vector>
#include <algorithm>
#include <Database.h>
#include <Interval.h>
#include <IntervalFactory.h>
#include <timew.h>

#include <TempDir.h>

const std::vector <Interval>& get_common_inputs ()
{
  static std::vector <Interval> inputs;
  if (inputs.empty ())
  {
    inputs.reserve (7);

    inputs.emplace_back (Datetime ("2016-06-03T01:00:00"), Datetime ("2016-06-03T02:00:00"));
    inputs.emplace_back (Datetime ("2018-06-02T01:00:00"), Datetime ("2018-06-02T02:00:00"));
    inputs.emplace_back (Datetime ("2019-06-03T01:00:00"), Datetime ("2019-06-03T02:00:00"));
    inputs.emplace_back (Datetime ("2017-06-02T01:00:00"), Datetime ("2017-06-02T02:00:00"));
    inputs.emplace_back (Datetime ("2018-06-03T01:00:00"), Datetime ("2018-06-03T02:00:00"));
    inputs.emplace_back (Datetime ("2019-06-02T01:00:00"), Datetime ("2019-06-02T02:00:00"));
    inputs.emplace_back (Datetime ("2016-06-02T01:00:00"), Datetime ("2016-06-02T02:00:00"));
  }

  return inputs;
}

bool test_database_remains_sorted ()
{
  const bool verbose = true;

  TempDir tempDir;

  const std::vector <Interval>& inputs = get_common_inputs ();

  Database database {".", 0};

  for (const auto& interval : inputs)
  {
    database.addInterval (interval, verbose);
  }

  database.commit ();
  if (!std::is_sorted (database.rbegin (), database.rend ()))
  {
    return false;
  }

  database = Database {".", 0};

  if (std::distance (database.begin(), database.end()) != static_cast <ssize_t> (inputs.size ()))
  {
    return false;
  }

  return true;
}

bool test_database_handles_add_on_reload ()
{
  const bool verbose = true;
  TempDir tempDir;

  const std::vector <Interval>& inputs = get_common_inputs ();

  // First, just load the database with the common inputs and commit
  Database database {".", 0};

  for (const auto& interval : inputs)
  {
    database.addInterval (interval, verbose);
  }
  database.commit ();

  // Reset the database
  database = Database {".", 0};

  database.addInterval ({Datetime ("1980-01-01T12:01:01"), Datetime ("1980-01-01T12:01:02")}, verbose);

  if (std::distance (database.begin(), database.end()) != static_cast <ssize_t> (inputs.size () + 1))
  {
    return false;
  }

  if (!std::is_sorted (database.rbegin (), database.rend ()))
  {
    return false;
  }

  return true;
}

bool test_database_forward_and_reverse_iterators_keep_ordering ()
{
  const bool verbose = true;
  TempDir tempDir;

  const std::vector <Interval>& inputs = get_common_inputs ();

  // First, just load the database with the common inputs and commit
  Database database {".", 0};

  for (const auto& interval : inputs)
  {
    database.addInterval (interval, verbose);
  }

  database.commit ();

  // Now reload database and check forward and reverse iterator
  database = Database {".", 0};

  std::vector <std::string> forward_vector {database.begin (), database.end ()};
  std::reverse (forward_vector.begin (), forward_vector.end ());

  return std::equal (database.rbegin (), database.rend (), forward_vector.begin ());
}

int main ()
{
  enableDebugMode (false);
  UnitTest t (3);
  t.ok (test_database_remains_sorted (), "database stays sorted when intervals are added out of order");
  t.ok (test_database_handles_add_on_reload (), "database remains sorted when mixing new intervals with those loaded from files");
  t.ok (test_database_forward_and_reverse_iterators_keep_ordering (), "database forward and reverse iterators keep same ordering");
  return 0;
}

