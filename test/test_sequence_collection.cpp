#define BOOST_TEST_MODULE sequence_collection

#include "test_generic_sequence.hpp"

#include "sequence_collection.hpp"
#include "sequence/indexed_sequence.hpp"
#include "sequence/partitioned_sequence.hpp"
#include "sequence/uniform_partitioned_sequence.hpp"
#include "succinct/mapper.hpp"

#include <vector>
#include <cstdlib>

template <typename BaseSequence>
void test_sequence_collection()
{
    ds2i::global_parameters params;
    uint64_t universe = 10000;
    typedef ds2i::sequence_collection<BaseSequence>
        collection_type;
    typename collection_type::builder b(params);

    std::vector<std::vector<uint64_t>> sequences(30);
    for (auto& seq: sequences) {
        double avg_gap = 1.1 + double(rand()) / RAND_MAX * 10;
        uint64_t n = uint64_t(universe / avg_gap);
        seq = random_sequence(universe, n, true);
        b.add_sequence(seq.begin(), seq.back() + 1, n);
    }

    {
        collection_type coll;
        b.build(coll);
        ds2i::mapper::freeze(coll, "temp.bin");
    }

    {
        collection_type coll;
        boost::iostreams::mapped_file_source m("temp.bin");
        ds2i::mapper::map(coll, m);

        for (size_t i = 0; i < sequences.size(); ++i) {
            test_sequence(coll[i], sequences[i]);
        }
    }
}

BOOST_AUTO_TEST_CASE(sequence_collection)
{
    test_sequence_collection<ds2i::indexed_sequence>();
    test_sequence_collection<ds2i::partitioned_sequence<>>();
    test_sequence_collection<ds2i::uniform_partitioned_sequence<>>();
}
