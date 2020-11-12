#include <boost/test/unit_test.hpp>
#include "../src/matching.h"

BOOST_AUTO_TEST_SUITE(MatchingTest)

BOOST_AUTO_TEST_CASE(basic_test)
{
    Matching matching(6);
    matching.add_to_matching(0, 1);
    matching.add_to_matching(2, 5);
    BOOST_CHECK(matching.contains_edge(0, 1));
    BOOST_CHECK(matching.contains_edge(2, 5));
    BOOST_CHECK(not matching.contains_edge(3, 4));
    BOOST_CHECK(not matching.is_matched(3));
    BOOST_CHECK(matching.is_matched(1));
    BOOST_CHECK(matching.size() == 2);
    std::vector<Matching::Edge> expected_edges{
            Matching::Edge(0, 1),
            Matching::Edge(2, 5),
    };
    BOOST_CHECK(matching.get_edges() == expected_edges);
}

BOOST_AUTO_TEST_CASE(test_augmentation)
{
    Matching matching(6);
    matching.add_to_matching(0, 1);
    matching.add_to_matching(2, 5);
    matching.augment_along({3, 0, 1, 2, 5, 4});
    for (NodeId i = 0; i < 6; ++i) {
        BOOST_CHECK(matching.is_matched(i));
    }
    std::vector<Matching::Edge> expected_edges{
            Matching::Edge(0, 3),
            Matching::Edge(1, 2),
            Matching::Edge(4, 5),
    };
    BOOST_CHECK(matching.get_edges() == expected_edges);
}

BOOST_AUTO_TEST_SUITE_END()
