/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>

#include "util/less.h"

#include "wn_types.h"

using namespace std;

namespace zorba {
namespace wordnet {

///////////////////////////////////////////////////////////////////////////////

namespace part_of_speech {
  type find( char pos ) {
    switch ( pos ) {
      case adjective:
      case adverb:
      case noun:
      case verb:
        return static_cast<type>( pos );
      default:
        return unknown;
    }
  }
} // namespace part_of_speech

///////////////////////////////////////////////////////////////////////////////

#define DEF_END(CHAR_ARRAY)                             \
  static char const *const *const end =                 \
    CHAR_ARRAY + sizeof( CHAR_ARRAY ) / sizeof( char* );

#define FIND(what) \
  static_cast<type>( find_index( string_of, end, what ) )

/**
 * A less-verbose way to use std::lower_bound.
 */ 
inline int find_index( char const *const *begin, char const *const *end,
                       char const *s ) {
  char const *const *const entry =
    ::lower_bound( begin, end, s, less<char const*>() );
  return entry != end && ::strcmp( s, *entry ) == 0 ? entry - begin : 0;
} 

namespace pointer {

  char const *const string_of[] = {
    "#UNKNOWN",
    "also see",
    "antonym",
    "attribute",
    "cause",
    "derivationally related form",
    "derived from adjective",
    "domain of synset region",
    "domain of synset topic",
    "domain of synset usage",
    "entailment",
    "hypernym",
    "hyponym",
    "instance hypernym",
    "instance hyponym",
    "member holonym",
    "member meronym",
    "member of domain region",
    "member of domain topic",
    "member of domain usage",
    "part holonym",
    "part meronym",
    "participle of verb",
    "pertainym",
    "similar to",
    "substance holonym",
    "substance meronym",
    "verb group"
  };

  type find( char ptr_symbol ) {
    switch ( ptr_symbol ) {
      case also_see:
      case antonym:
      case attribute:
      case cause:
      case derivationally_related_form:
      case derived_from_adjective:
      case domain_of_synset_region:
      case domain_of_synset_topic:
      case domain_of_synset_usage:
      case entailment:
      case hypernym:
      case hyponym:
      case instance_hypernym:
      case instance_hyponym:
      case member_holonym:
      case member_meronym:
      case member_of_domain_region:
      case member_of_domain_topic:
      case member_of_domain_usage:
      case part_holonym:
      case participle_of_verb:
      case part_meronym:
      case pertainym:
      case similar_to:
      case substance_holonym:
      case substance_meronym:
      case verb_group:
        return static_cast<type>( ptr_symbol );
      default:
        return unknown;
    }
  }

  type find( char const *relationship ) {
    DEF_END( string_of );
    return FIND( relationship );
  }

  type map_iso_rel( iso2788::rel_type iso_rel ) {
    switch ( iso_rel ) {
      case iso2788::BT : return hypernym;
      case iso2788::BTG:
      case iso2788::BTP:
      case iso2788::NT : return hyponym;
      case iso2788::NTG:
      case iso2788::NTP:
      case iso2788::RT : return similar_to;
      case iso2788::TT :
      case iso2788::UF :
      case iso2788::USE:
      default          : return unknown;
    }
  }

} // namespace pointer

///////////////////////////////////////////////////////////////////////////////

} // namespace wordnet
} // namespace zorba

/* vim:set et sw=2 ts=2: */
