
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEMPLATE_TEST_MACROS_HPP_INCLUDED
#define CATCH_TEMPLATE_TEST_MACROS_HPP_INCLUDED

// We need this suppression to leak, because it took until GCC 10
// for the front end to handle local suppression via _Pragma properly
// inside templates (so `TEMPLATE_TEST_CASE` and co).
// **THIS IS DIFFERENT FOR STANDARD TESTS, WHERE GCC 9 IS SUFFICIENT**
#if defined( __GNUC__ ) && !defined( __clang__ ) && !defined( __ICC ) && \
    __GNUC__ < 10
#    pragma GCC diagnostic ignored "-Wparentheses"
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_preprocessor.hpp>
#include <catch2/internal/catch_template_test_registry.hpp>

#if defined( CATCH_CONFIG_PREFIX_ALL ) && !defined( CATCH_CONFIG_DISABLE )

#    ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
#        define CATCH_TEMPLATE_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE( __VA_ARGS__ )
#        define CATCH_TEMPLATE_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG( __VA_ARGS__ )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD( className, ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className, ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className,  \
                                                          __VA_ARGS__ )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE( __VA_ARGS__ )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG( __VA_ARGS__ )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD( className, ... ) \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD( className,  \
                                                              __VA_ARGS__ )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( className, ... ) \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(             \
                className, __VA_ARGS__ )
#        define CATCH_TEMPLATE_LIST_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE( __VA_ARGS__ )
#        define CATCH_TEMPLATE_LIST_TEST_CASE_METHOD( className, ... ) \
            INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_METHOD( className,  \
                                                           __VA_ARGS__ )
#    else
#        define CATCH_TEMPLATE_TEST_CASE( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(        \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE( __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(            \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG( __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD( className, ... )    \
            INTERNAL_CATCH_EXPAND_VARGS(                             \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( className, \
                                                          __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className, ... )    \
            INTERNAL_CATCH_EXPAND_VARGS(                                 \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className, \
                                                              __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(                \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE( __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(                    \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG( __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD( className, ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(                                  \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD(         \
                    className, __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( className, ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(                                      \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(         \
                    className, __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_LIST_TEST_CASE( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(             \
                INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE( __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_LIST_TEST_CASE_METHOD( className, ... )    \
            INTERNAL_CATCH_EXPAND_VARGS(                                  \
                INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_METHOD( className, \
                                                               __VA_ARGS__ ) )
#    endif

#elif defined( CATCH_CONFIG_PREFIX_ALL ) && defined( CATCH_CONFIG_DISABLE )

#    ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
#        define CATCH_TEMPLATE_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION( __VA_ARGS__ )
#        define CATCH_TEMPLATE_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG_NO_REGISTRATION( __VA_ARGS__ )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD( className, ... )     \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION( \
                className, __VA_ARGS__ )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className, ... )     \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG_NO_REGISTRATION( \
                className, __VA_ARGS__ )
#    else
#        define CATCH_TEMPLATE_TEST_CASE( ... )                    \
            INTERNAL_CATCH_EXPAND_VARGS(                           \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION( \
                    __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_TEST_CASE_SIG( ... )                    \
            INTERNAL_CATCH_EXPAND_VARGS(                               \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG_NO_REGISTRATION( \
                    __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD( className, ... )         \
            INTERNAL_CATCH_EXPAND_VARGS(                                  \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION( \
                    className, __VA_ARGS__ ) )
#        define CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className, ... )         \
            INTERNAL_CATCH_EXPAND_VARGS(                                      \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG_NO_REGISTRATION( \
                    className, __VA_ARGS__ ) )
#    endif

// When disabled, these can be shared between proper preprocessor and MSVC
// preprocessor
#    define CATCH_TEMPLATE_PRODUCT_TEST_CASE( ... ) \
        CATCH_TEMPLATE_TEST_CASE( __VA_ARGS__ )
#    define CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG( ... ) \
        CATCH_TEMPLATE_TEST_CASE( __VA_ARGS__ )
#    define CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD( className, ... ) \
        CATCH_TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )
#    define CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( className, ... ) \
        CATCH_TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )
#    define CATCH_TEMPLATE_LIST_TEST_CASE( ... ) \
        CATCH_TEMPLATE_TEST_CASE( __VA_ARGS__ )
#    define CATCH_TEMPLATE_LIST_TEST_CASE_METHOD( className, ... ) \
        CATCH_TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )

#elif !defined( CATCH_CONFIG_PREFIX_ALL ) && !defined( CATCH_CONFIG_DISABLE )

#    ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
#        define TEMPLATE_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE( __VA_ARGS__ )
#        define TEMPLATE_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG( __VA_ARGS__ )
#        define TEMPLATE_TEST_CASE_METHOD( className, ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )
#        define TEMPLATE_TEST_CASE_METHOD_SIG( className, ... )      \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className, \
                                                          __VA_ARGS__ )
#        define TEMPLATE_PRODUCT_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE( __VA_ARGS__ )
#        define TEMPLATE_PRODUCT_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG( __VA_ARGS__ )
#        define TEMPLATE_PRODUCT_TEST_CASE_METHOD( className, ... )      \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD( className, \
                                                              __VA_ARGS__ )
#        define TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( className, ... ) \
            INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(       \
                className, __VA_ARGS__ )
#        define TEMPLATE_LIST_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE( __VA_ARGS__ )
#        define TEMPLATE_LIST_TEST_CASE_METHOD( className, ... )      \
            INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_METHOD( className, \
                                                           __VA_ARGS__ )
#    else
#        define TEMPLATE_TEST_CASE( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(  \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE( __VA_ARGS__ ) )
#        define TEMPLATE_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(      \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG( __VA_ARGS__ ) )
#        define TEMPLATE_TEST_CASE_METHOD( className, ... )          \
            INTERNAL_CATCH_EXPAND_VARGS(                             \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( className, \
                                                          __VA_ARGS__ ) )
#        define TEMPLATE_TEST_CASE_METHOD_SIG( className, ... )          \
            INTERNAL_CATCH_EXPAND_VARGS(                                 \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( className, \
                                                              __VA_ARGS__ ) )
#        define TEMPLATE_PRODUCT_TEST_CASE( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(          \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE( __VA_ARGS__ ) )
#        define TEMPLATE_PRODUCT_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(              \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG( __VA_ARGS__ ) )
#        define TEMPLATE_PRODUCT_TEST_CASE_METHOD( className, ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(                            \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD(   \
                    className, __VA_ARGS__ ) )
#        define TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( className, ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(                                \
                INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(   \
                    className, __VA_ARGS__ ) )
#        define TEMPLATE_LIST_TEST_CASE( ... ) \
            INTERNAL_CATCH_EXPAND_VARGS(       \
                INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE( __VA_ARGS__ ) )
#        define TEMPLATE_LIST_TEST_CASE_METHOD( className, ... )          \
            INTERNAL_CATCH_EXPAND_VARGS(                                  \
                INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_METHOD( className, \
                                                               __VA_ARGS__ ) )
#    endif

#elif !defined( CATCH_CONFIG_PREFIX_ALL ) && defined( CATCH_CONFIG_DISABLE )

#    ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
#        define TEMPLATE_TEST_CASE( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION( __VA_ARGS__ )
#        define TEMPLATE_TEST_CASE_SIG( ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG_NO_REGISTRATION( __VA_ARGS__ )
#        define TEMPLATE_TEST_CASE_METHOD( className, ... )           \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION( \
                className, __VA_ARGS__ )
#        define TEMPLATE_TEST_CASE_METHOD_SIG( className, ... )           \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG_NO_REGISTRATION( \
                className, __VA_ARGS__ )
#    else
#        define TEMPLATE_TEST_CASE( ... )                          \
            INTERNAL_CATCH_EXPAND_VARGS(                           \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION( \
                    __VA_ARGS__ ) )
#        define TEMPLATE_TEST_CASE_SIG( ... )                          \
            INTERNAL_CATCH_EXPAND_VARGS(                               \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG_NO_REGISTRATION( \
                    __VA_ARGS__ ) )
#        define TEMPLATE_TEST_CASE_METHOD( className, ... )               \
            INTERNAL_CATCH_EXPAND_VARGS(                                  \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION( \
                    className, __VA_ARGS__ ) )
#        define TEMPLATE_TEST_CASE_METHOD_SIG( className, ... )               \
            INTERNAL_CATCH_EXPAND_VARGS(                                      \
                INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG_NO_REGISTRATION( \
                    className, __VA_ARGS__ ) )
#    endif

// When disabled, these can be shared between proper preprocessor and MSVC
// preprocessor
#    define TEMPLATE_PRODUCT_TEST_CASE( ... ) TEMPLATE_TEST_CASE( __VA_ARGS__ )
#    define TEMPLATE_PRODUCT_TEST_CASE_SIG( ... ) \
        TEMPLATE_TEST_CASE( __VA_ARGS__ )
#    define TEMPLATE_PRODUCT_TEST_CASE_METHOD( className, ... ) \
        TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )
#    define TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( className, ... ) \
        TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )
#    define TEMPLATE_LIST_TEST_CASE( ... ) TEMPLATE_TEST_CASE( __VA_ARGS__ )
#    define TEMPLATE_LIST_TEST_CASE_METHOD( className, ... ) \
        TEMPLATE_TEST_CASE_METHOD( className, __VA_ARGS__ )

#endif // end of user facing macro declarations

#endif // CATCH_TEMPLATE_TEST_MACROS_HPP_INCLUDED
