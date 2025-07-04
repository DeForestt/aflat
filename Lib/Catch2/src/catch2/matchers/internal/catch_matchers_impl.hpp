
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MATCHERS_IMPL_HPP_INCLUDED
#define CATCH_MATCHERS_IMPL_HPP_INCLUDED

#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <catch2/internal/catch_test_macro_impl.hpp>

namespace Catch {

    template <typename ArgT, typename MatcherT>
    class MatchExpr : public ITransientExpression {
        ArgT&& m_arg;
        MatcherT const& m_matcher;
        StringRef m_matcherString;

    public:
        MatchExpr( ArgT&& arg,
                   MatcherT const& matcher,
                   StringRef matcherString ):
            ITransientExpression{
                true,
                matcher.match( arg ) }, // not forwarding arg here on purpose
            m_arg( CATCH_FORWARD( arg ) ),
            m_matcher( matcher ),
            m_matcherString( matcherString ) {}

        void streamReconstructedExpression( std::ostream& os ) const override {
            os << Catch::Detail::stringify( m_arg ) << ' '
               << m_matcher.toString();
        }
    };

    namespace Matchers {
        template <typename ArgT> class MatcherBase;
    }

    using StringMatcher = Matchers::MatcherBase<std::string>;

    void handleExceptionMatchExpr( AssertionHandler& handler,
                                   StringMatcher const& matcher,
                                   StringRef matcherString );

    template <typename ArgT, typename MatcherT>
    auto makeMatchExpr( ArgT&& arg,
                        MatcherT const& matcher,
                        StringRef matcherString ) -> MatchExpr<ArgT, MatcherT> {
        return MatchExpr<ArgT, MatcherT>(
            CATCH_FORWARD( arg ), matcher, matcherString );
    }

} // namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( macroName, matcher, resultDisposition, arg )    \
    do {                                                                     \
        Catch::AssertionHandler catchAssertionHandler(                       \
            macroName##_catch_sr,                                            \
            CATCH_INTERNAL_LINEINFO,                                         \
            CATCH_INTERNAL_STRINGIFY( arg ) ", " CATCH_INTERNAL_STRINGIFY(   \
                matcher ),                                                   \
            resultDisposition );                                             \
        INTERNAL_CATCH_TRY {                                                 \
            catchAssertionHandler.handleExpr(                                \
                Catch::makeMatchExpr( arg, matcher, #matcher##_catch_sr ) ); \
        }                                                                    \
        INTERNAL_CATCH_CATCH( catchAssertionHandler )                        \
        INTERNAL_CATCH_REACT( catchAssertionHandler )                        \
    } while ( false )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_MATCHES(                                          \
    macroName, exceptionType, resultDisposition, matcher, ... )                 \
    do {                                                                        \
        Catch::AssertionHandler catchAssertionHandler(                          \
            macroName##_catch_sr,                                               \
            CATCH_INTERNAL_LINEINFO,                                            \
            CATCH_INTERNAL_STRINGIFY(                                           \
                __VA_ARGS__ ) ","                                               \
                              " " CATCH_INTERNAL_STRINGIFY(                     \
                                  exceptionType ) ","                           \
                                                  " " CATCH_INTERNAL_STRINGIFY( \
                                                      matcher ),                \
            resultDisposition );                                                \
        if ( catchAssertionHandler.allowThrows() )                              \
            try {                                                               \
                static_cast<void>( __VA_ARGS__ );                               \
                catchAssertionHandler.handleUnexpectedExceptionNotThrown();     \
            } catch ( exceptionType const& ex ) {                               \
                catchAssertionHandler.handleExpr( Catch::makeMatchExpr(         \
                    ex, matcher, #matcher##_catch_sr ) );                       \
            } catch ( ... ) {                                                   \
                catchAssertionHandler.handleUnexpectedInflightException();      \
            }                                                                   \
        else                                                                    \
            catchAssertionHandler.handleThrowingCallSkipped();                  \
        INTERNAL_CATCH_REACT( catchAssertionHandler )                           \
    } while ( false )

#endif // CATCH_MATCHERS_IMPL_HPP_INCLUDED
