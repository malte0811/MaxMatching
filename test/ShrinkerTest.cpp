#include <boost/test/unit_test.hpp>
#include "../src/nested_shrunken_graph.h"

BOOST_AUTO_TEST_SUITE(ShrinkerTest)

BOOST_AUTO_TEST_CASE(basic_test)
{
    NestedShrunkenGraph shrinker(6);
    for (NestedShrunkenGraph::Representative i = 0; i < 6; ++i) {
        BOOST_CHECK(shrinker.get_representative(i) == i);
    }
    BOOST_CHECK(not shrinker.is_shrunken());
    std::vector<NestedShrunkenGraph::Representative> to_shrink{0, 2, 4};
    auto const& circuit_repr = shrinker.shrink(to_shrink);
    BOOST_CHECK(shrinker.get_representative(0) == circuit_repr);
    BOOST_CHECK(shrinker.get_representative(1) == 1);
    BOOST_CHECK(shrinker.get_representative(2) == circuit_repr);
    BOOST_CHECK(shrinker.get_representative(3) == 3);
    BOOST_CHECK(shrinker.get_representative(4) == circuit_repr);
    BOOST_CHECK(shrinker.get_representative(5) == 5);
    BOOST_CHECK(shrinker.is_shrunken());
    auto const&[shrunken_set, shrunken_id] = shrinker.expand();
    BOOST_CHECK(shrunken_id == circuit_repr);
    BOOST_CHECK(shrunken_set == to_shrink);
    BOOST_CHECK(not shrinker.is_shrunken());
    for (NestedShrunkenGraph::Representative i = 0; i < 6; ++i) {
        BOOST_CHECK(shrinker.get_representative(i) == i);
    }
}

BOOST_AUTO_TEST_SUITE_END()
