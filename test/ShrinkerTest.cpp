#include <boost/test/unit_test.hpp>
#include "../src/nested_shrinking.h"

BOOST_AUTO_TEST_SUITE(ShrinkerTest)

BOOST_AUTO_TEST_CASE(basic_test)
{
    NestedShrinking shrinker(6);
    for (NodeId i = 0; i < 6; ++i) {
        BOOST_CHECK(shrinker.get_representative(i) == Representative(i));
    }
    BOOST_CHECK(not shrinker.is_shrunken());
    std::vector<Representative> to_shrink{
        Representative(0),
        Representative(2),
        Representative(4)
    };
    auto const& circuit_repr = shrinker.shrink(to_shrink);
    BOOST_CHECK(shrinker.get_representative(0) == circuit_repr);
    BOOST_CHECK(shrinker.get_representative(1).id() == 1);
    BOOST_CHECK(shrinker.get_representative(2) == circuit_repr);
    BOOST_CHECK(shrinker.get_representative(3).id() == 3);
    BOOST_CHECK(shrinker.get_representative(4) == circuit_repr);
    BOOST_CHECK(shrinker.get_representative(5).id() == 5);
    BOOST_CHECK(shrinker.is_shrunken());
    auto const&[shrunken_set, shrunken_id] = shrinker.expand();
    BOOST_CHECK(shrunken_id == circuit_repr);
    BOOST_CHECK(shrunken_set == to_shrink);
    BOOST_CHECK(not shrinker.is_shrunken());
    for (NodeId i = 0; i < 6; ++i) {
        BOOST_CHECK(shrinker.get_representative(i).id() == i);
    }
}

BOOST_AUTO_TEST_SUITE_END()
