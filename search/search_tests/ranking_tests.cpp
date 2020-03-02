#include "testing/testing.hpp"

#include "search/query_params.hpp"
#include "search/ranking_utils.hpp"
#include "search/token_range.hpp"
#include "search/token_slice.hpp"

#include "indexer/search_delimiters.hpp"
#include "indexer/search_string_utils.hpp"

#include "base/stl_helpers.hpp"
#include "base/string_utils.hpp"

#include <cstdint>
#include <string>
#include <vector>

using namespace search;
using namespace std;
using namespace strings;

namespace
{
NameScores GetScore(string const & name, string const & query, TokenRange const & tokenRange)
{
  search::Delimiters delims;
  QueryParams params;

  vector<UniString> tokens;
  SplitUniString(NormalizeAndSimplifyString(query), base::MakeBackInsertFunctor(tokens), delims);

  if (!query.empty() && !delims(strings::LastUniChar(query)))
  {
    CHECK(!tokens.empty(), ());
    params.InitWithPrefix(tokens.begin(), tokens.end() - 1, tokens.back());
  }
  else
  {
    params.InitNoPrefix(tokens.begin(), tokens.end());
  }

  return GetNameScores(name, TokenSlice(params, tokenRange));
}

UNIT_TEST(NameTest_Smoke)
{
  auto const test = [](string const & name, string const & query, TokenRange const & tokenRange,
                       NameScore nameScore, size_t errorsMade) {
    TEST_EQUAL(
        GetScore(name, query, tokenRange),
        NameScores(nameScore, nameScore == NAME_SCORE_ZERO ? ErrorsMade() : ErrorsMade(errorsMade)),
        (name, query, tokenRange));
  };

  test("New York", "Central Park, New York, US", TokenRange(2, 4), NAME_SCORE_FULL_MATCH, 0);
  test("New York", "York", TokenRange(0, 1), NAME_SCORE_SUBSTRING, 0);
  test("Moscow", "Red Square Mosc", TokenRange(2, 3), NAME_SCORE_PREFIX, 0);
  test("Moscow", "Red Square Moscow", TokenRange(2, 3), NAME_SCORE_FULL_MATCH, 0);
  test("Moscow", "Red Square Moscw", TokenRange(2, 3), NAME_SCORE_FULL_MATCH, 1);
  test("San Francisco", "Fran", TokenRange(0, 1), NAME_SCORE_SUBSTRING, 0);
  test("San Francisco", "Fran ", TokenRange(0, 1), NAME_SCORE_ZERO, 0);
  test("San Francisco", "Sa", TokenRange(0, 1), NAME_SCORE_PREFIX, 0);
  test("San Francisco", "San ", TokenRange(0, 1), NAME_SCORE_PREFIX, 0);
  test("Лермонтовъ", "Лермон", TokenRange(0, 1), NAME_SCORE_PREFIX, 0);
  test("Лермонтовъ", "Лермонтов", TokenRange(0, 1), NAME_SCORE_FULL_MATCH, 1);
  test("Лермонтовъ", "Лермонтово", TokenRange(0, 1), NAME_SCORE_FULL_MATCH, 1);
  test("Лермонтовъ", "Лермнтовъ", TokenRange(0, 1), NAME_SCORE_FULL_MATCH, 1);
  test("фото на документы", "фото", TokenRange(0, 1), NAME_SCORE_PREFIX, 0);
  test("фотоателье", "фото", TokenRange(0, 1), NAME_SCORE_PREFIX, 0);
}
}  // namespace