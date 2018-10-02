// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_CONTRIB_UNIQUE_PTR_TRAITS_HPP
#define TAO_JSON_CONTRIB_UNIQUE_PTR_TRAITS_HPP

#include <memory>

#include "../forward.hpp"

#include "internal/indirect_traits.hpp"
#include "internal/type_traits.hpp"

namespace tao
{
   namespace json
   {
      namespace internal
      {
         template< typename T, typename U >
         struct unique_ptr_traits
         {
            template< template< typename... > class Traits, typename Base, typename... With >
            static auto as( const basic_value< Traits, Base >& v, const With&... with ) -> typename std::enable_if< use_first_ptr_as< T, Traits, Base, With... >::value, std::unique_ptr< U > >::type
            {
               if( v == null ) {
                  return std::unique_ptr< U >();
               }
               return std::unique_ptr< U >( new T( v, with... ) );
            }

            template< template< typename... > class Traits, typename Base, typename... With >
            static auto as( const basic_value< Traits, Base >& v, const With&... with ) -> typename std::enable_if< use_second_ptr_as< T, Traits, Base, With... >::value || use_fourth_ptr_as< T, Traits, Base, With... >::value, std::unique_ptr< U > >::type
            {
               if( v == null ) {
                  return std::unique_ptr< U >();
               }
               return std::unique_ptr< U >( new T( Traits< T >::as( v, with... ) ) );
            }

            template< template< typename... > class Traits, typename Base, typename... With >
            static auto as( const basic_value< Traits, Base >& v, const With&... with ) -> typename std::enable_if< use_third_ptr_as< T, Traits, Base, With... >::value, std::unique_ptr< U > >::type
            {
               if( v == null ) {
                  return std::unique_ptr< U >();
               }
               std::unique_ptr< U > t( new T() );
               Traits< T >::to( v, static_cast< T& >( *t ), with... );
               return t;
            }

            template< template< typename... > class Traits, typename Producer >
            static auto consume( Producer& parser ) -> typename std::enable_if< use_first_ptr_consume< T, Traits, Producer >::value || use_third_ptr_consume< T, Traits, Producer >::value, std::unique_ptr< U > >::type
            {
               if( parser.null() ) {
                  return std::unique_ptr< U >();
               }
               return Traits< T >::template consume< Traits >( parser );
            }

            template< template< typename... > class Traits, typename Producer >
            static auto consume( Producer& parser ) -> typename std::enable_if< use_second_ptr_consume< T, Traits, Producer >::value, std::unique_ptr< U > >::type
            {
               if( parser.null() ) {
                  return std::unique_ptr< U >();
               }
               std::unique_ptr< U > t( new T() );
               Traits< T >::template consume< Traits >( parser, static_cast< T& >( *t ) );
               return t;
            }
         };

      }  // namespace internal

      template< typename T, typename U = T >
      struct unique_ptr_traits
         : public internal::unique_ptr_traits< T, U >
      {
         template< template< typename... > class Traits, typename Base >
         static void assign( basic_value< Traits, Base >& v, const std::unique_ptr< U >& o )
         {
            v = static_cast< const T& >( *o );  // Unconditional dereference for use in binding::factory.
         }

         template< template< typename... > class Traits, typename Consumer >
         static void produce( Consumer& c, const std::unique_ptr< U >& o )
         {
            json::events::produce< Traits >( c, static_cast< const T& >( *o ) );  // Unconditional dereference for use in binding::factory.
         }
      };

      template< typename T >
      struct unique_ptr_traits< T, T >
         : public internal::unique_ptr_traits< T, T >,
           public internal::indirect_traits< std::unique_ptr< T > >
      {
         template< typename V >
         using with_base = unique_ptr_traits< T, V >;
      };

   }  // namespace json

}  // namespace tao

#endif