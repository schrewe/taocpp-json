// Copyright (c) 2017 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAOCPP_JSON_INCLUDE_JAXN_PARSE_FILE_HPP
#define TAOCPP_JSON_INCLUDE_JAXN_PARSE_FILE_HPP

#include <string>
#include <utility>

#include "../events/jaxn/parse_file.hpp"
#include "../events/to_value.hpp"
#include "../events/transformer.hpp"

namespace tao
{
   namespace json
   {
      namespace jaxn
      {
         template< template< typename... > class Traits, template< typename... > class... Transformers >
         basic_value< Traits > basic_parse_file( const std::string& filename )
         {
            events::transformer< events::to_basic_value< Traits >, Transformers... > consumer;
            events::jaxn::parse_file( filename, consumer );
            return std::move( consumer.value );
         }

         template< template< typename... > class... Transformers, template< typename... > class Traits >
         void parse_file( basic_value< Traits >& output, const std::string& filename )
         {
            output = basic_parse_file< Traits, Transformers... >( filename );
         }

         template< template< typename... > class... Transformers >
         value parse_file( const std::string& filename )
         {
            return basic_parse_file< traits, Transformers... >( filename );
         }

      }  // namespace jaxn

   }  // namespace json

}  // namespace tao

#endif