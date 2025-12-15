/**
 * @file myyaml.cppm
 * @brief File containing the module declaration for myyaml.
 */

module;

#include <myyaml/myyaml.hpp>

export module myyaml;

/**
 * @namespace myyaml
 * @brief The myyaml namespace myyaml::
 */
export namespace myyaml {

/**
 * @namespace literals
 * @brief The myyaml namespace myyaml::literals::
 */
inline namespace literals {

using ::myyaml::literals::operator""_yaml;

}  // namespace literals

using ::myyaml::yaml;

}  // namespace myyaml
