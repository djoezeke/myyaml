/**
 * @file Main.cpp
 * @brief Catch2 microbenchmarks for the myyaml parser and emitter.
 *
 * The benchmarks use Catch2's BENCHMARK macro so they can be run together
 * with the unit tests or as a standalone benchmark suite. Every benchmark
 * works on the same realistic, pre-generated documents to keep the numbers
 * comparable between runs.
 *
 * Run all benchmarks:
 *   myyaml-benchmarks
 *
 * Run a subset:
 *   myyaml-benchmarks "[parser]"
 *   myyaml-benchmarks "[emitter]"
 *   myyaml-benchmarks "[c-api]"
 *
 * Reduce the runtime while developing:
 *   myyaml-benchmarks --benchmark-samples 10 --benchmark-resamples 1000
 */

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <myyaml/myyaml.hpp>

#if defined(MYYAML_BENCHMARKS_HAVE_C_API)
#include <myyaml/myyaml.h>
#endif

#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
    constexpr std::string_view small_document =
        "name: myyaml\n"
        "version: 0.2.0\n"
        "enabled: true\n"
        "retries: 3\n"
        "ratio: 0.75\n"
        "features: [parse, emit, anchors]\n"
        "nested:\n"
        "  key: value\n"
        "  list:\n"
        "    - 1\n"
        "    - 2\n";

    constexpr std::string_view flow_document =
        "{name: myyaml, version: 0.2.0, tags: [c, cpp, yaml], "
        "limits: {depth: 128, nodes: 1000000}, enabled: true}";

    constexpr std::string_view alias_document =
        "defaults: &defaults\n"
        "  timeout: 30\n"
        "  retries: 3\n"
        "  endpoints: [primary, secondary]\n"
        "primary: *defaults\n"
        "secondary: *defaults\n";

    std::string make_long_sequence(std::size_t entries)
    {
        std::ostringstream stream;
        stream << "values:\n";
        for (std::size_t index = 0; index < entries; ++index)
        {
            stream << "  - " << index << '\n';
        }
        return stream.str();
    }

    std::string make_service_config(std::size_t services)
    {
        std::ostringstream stream;
        stream << "apiVersion: v1\n"
                  "kind: ServiceSet\n"
                  "metadata:\n"
                  "  name: myyaml-benchmark\n"
                  "  labels:\n"
                  "    app: myyaml\n"
                  "    tier: backend\n"
                  "spec:\n"
                  "  replicas: 3\n"
                  "  description: >-\n"
                  "    A generated configuration used to measure parser and\n"
                  "    emitter throughput on a realistic nested document.\n"
                  "  services:\n";
        for (std::size_t index = 0; index < services; ++index)
        {
            stream << "    - name: service-" << index << '\n'
                   << "      image: registry.example.com/service:" << (index % 17)
                   << ".0\n"
                   << "      replicas: " << (index % 5 + 1) << '\n'
                   << "      enabled: " << (index % 2 == 0 ? "true" : "false") << '\n'
                   << "      ports:\n"
                   << "        - containerPort: " << (8000 + index) << '\n'
                   << "          protocol: TCP\n"
                   << "      env:\n"
                   << "        - name: LOG_LEVEL\n"
                   << "          value: info\n"
                   << "        - name: SHARD\n"
                   << "          value: \"" << (index % 32) << "\"\n"
                   << "      resources:\n"
                   << "        limits: {cpu: \"" << (index % 4 + 1) << "\", memory: \""
                   << (index % 8 + 1) << "Gi\"}\n";
        }
        return stream.str();
    }

    std::string make_mapping(std::size_t entries)
    {
        std::ostringstream stream;
        for (std::size_t index = 0; index < entries; ++index)
        {
            stream << "key-" << index << ": " << index << '\n';
        }
        return stream.str();
    }

    struct benchmark_documents
    {
        std::string small{small_document};
        std::string flow{flow_document};
        std::string aliases{alias_document};
        std::string long_sequence{make_long_sequence(1000)};
        std::string large_config{make_service_config(250)};
        std::string wide_mapping{make_mapping(256)};

        myyaml::yaml parsed_small{myyaml::yaml::parse(small)};
        myyaml::yaml parsed_flow{myyaml::yaml::parse(flow)};
        myyaml::yaml parsed_large{myyaml::yaml::parse(large_config)};
        myyaml::yaml parsed_mapping{myyaml::yaml::parse(wide_mapping)};
    };

    const benchmark_documents &documents()
    {
        static const benchmark_documents value;
        return value;
    }

    myyaml::dump_options flow_options()
    {
        myyaml::dump_options options;
        options.flow = true;
        return options;
    }

    myyaml::dump_options sorted_options()
    {
        myyaml::dump_options options;
        options.sort_keys = true;
        options.indent = 4;
        return options;
    }
} // namespace

TEST_CASE("parser throughput", "[benchmark][parser]")
{
    const benchmark_documents &data = documents();

    BENCHMARK("parse: scalar integer")
    {
        return myyaml::yaml::parse("42").as_integer();
    };

    BENCHMARK("parse: small block document")
    {
        return myyaml::yaml::parse(data.small);
    };

    BENCHMARK("parse: flow document")
    {
        return myyaml::yaml::parse(data.flow);
    };

    BENCHMARK("parse: anchors and aliases")
    {
        return myyaml::yaml::parse(data.aliases);
    };

    BENCHMARK("parse: 1000 element sequence")
    {
        return myyaml::yaml::parse(data.long_sequence);
    };

    BENCHMARK("parse: 256 key mapping")
    {
        return myyaml::yaml::parse(data.wide_mapping);
    };

    BENCHMARK("parse: large nested configuration")
    {
        return myyaml::yaml::parse(data.large_config);
    };
}

TEST_CASE("emitter throughput", "[benchmark][emitter]")
{
    const benchmark_documents &data = documents();

    BENCHMARK("emit: scalar")
    {
        return myyaml::yaml(3.141592653589793).dump();
    };

    BENCHMARK("emit: small block document")
    {
        return data.parsed_small.dump();
    };

    BENCHMARK("emit: small flow document")
    {
        return data.parsed_small.dump(flow_options());
    };

    BENCHMARK("emit: 256 key mapping")
    {
        return data.parsed_mapping.dump();
    };

    BENCHMARK("emit: 256 key mapping (sorted, indent 4)")
    {
        return data.parsed_mapping.dump(sorted_options());
    };

    BENCHMARK("emit: large nested configuration")
    {
        return data.parsed_large.dump();
    };

    BENCHMARK("emit: large nested configuration (flow)")
    {
        return data.parsed_large.dump(flow_options());
    };
}

TEST_CASE("round-trip and access throughput", "[benchmark][roundtrip]")
{
    const benchmark_documents &data = documents();

    BENCHMARK("round-trip: small block document")
    {
        return myyaml::yaml::parse(data.small).dump();
    };

    BENCHMARK("round-trip: large nested configuration")
    {
        return myyaml::yaml::parse(data.large_config).dump();
    };

    BENCHMARK("lookup: 256 key mapping")
    {
        std::size_t found = 0;
        for (std::size_t index = 0; index < 256; ++index)
        {
            found += data.parsed_mapping.contains("key-" + std::to_string(index)) ? 1 : 0;
        }
        return found;
    };

    BENCHMARK("mutate: insert 64 mapping entries")
    {
        myyaml::yaml object = myyaml::yaml::make_mapping();
        for (std::size_t index = 0; index < 64; ++index)
        {
            object.set("key-" + std::to_string(index), index);
        }
        return object;
    };

    BENCHMARK("mutate: append 128 sequence entries")
    {
        myyaml::yaml list = myyaml::yaml::make_sequence();
        for (std::size_t index = 0; index < 128; ++index)
        {
            list.push_back(index);
        }
        return list;
    };
}

#if defined(MYYAML_BENCHMARKS_HAVE_C_API)
TEST_CASE("C API throughput", "[benchmark][c-api]")
{
    const benchmark_documents &data = documents();

    BENCHMARK("C API: parse large configuration")
    {
        myyaml_document *document =
            myyaml_parse_n(data.large_config.data(), data.large_config.size());
        if (document == nullptr)
        {
            return std::size_t{0};
        }
        myyaml_free(document);
        return data.large_config.size();
    };

    BENCHMARK("C API: parse + dump large configuration")
    {
        myyaml_document *document =
            myyaml_parse_n(data.large_config.data(), data.large_config.size());
        if (document == nullptr)
        {
            return std::size_t{0};
        }
        char *text = myyaml_dump(document);
        const std::size_t size =
            text == nullptr ? 0 : std::char_traits<char>::length(text);
        myyaml_free_string(text);
        myyaml_free(document);
        return size;
    };
}
#endif
