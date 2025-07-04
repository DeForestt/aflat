
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <algorithm>
#include <cassert>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/internal/catch_test_case_tracker.hpp>

#if defined( __clang__ )
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

namespace Catch {
    namespace TestCaseTracking {

        NameAndLocation::NameAndLocation( std::string const& _name,
                                          SourceLineInfo const& _location ):
            name( _name ), location( _location ) {}

        ITracker::~ITracker() = default;

        void ITracker::markAsNeedingAnotherRun() {
            m_runState = NeedsAnotherRun;
        }

        void ITracker::addChild( ITrackerPtr&& child ) {
            m_children.push_back( CATCH_MOVE( child ) );
        }

        ITracker*
        ITracker::findChild( NameAndLocation const& nameAndLocation ) {
            auto it =
                std::find_if( m_children.begin(),
                              m_children.end(),
                              [&nameAndLocation]( ITrackerPtr const& tracker ) {
                                  return tracker->nameAndLocation().location ==
                                             nameAndLocation.location &&
                                         tracker->nameAndLocation().name ==
                                             nameAndLocation.name;
                              } );
            return ( it != m_children.end() ) ? it->get() : nullptr;
        }

        bool ITracker::isSectionTracker() const { return false; }
        bool ITracker::isGeneratorTracker() const { return false; }

        bool ITracker::isSuccessfullyCompleted() const {
            return m_runState == CompletedSuccessfully;
        }

        bool ITracker::isOpen() const {
            return m_runState != NotStarted && !isComplete();
        }

        bool ITracker::hasStarted() const { return m_runState != NotStarted; }

        void ITracker::openChild() {
            if ( m_runState != ExecutingChildren ) {
                m_runState = ExecutingChildren;
                if ( m_parent ) {
                    m_parent->openChild();
                }
            }
        }

        ITracker& TrackerContext::startRun() {
            using namespace std::string_literals;
            m_rootTracker = Catch::Detail::make_unique<SectionTracker>(
                NameAndLocation( "{root}"s, CATCH_INTERNAL_LINEINFO ),
                *this,
                nullptr );
            m_currentTracker = nullptr;
            m_runState = Executing;
            return *m_rootTracker;
        }

        void TrackerContext::endRun() {
            m_rootTracker.reset();
            m_currentTracker = nullptr;
            m_runState = NotStarted;
        }

        void TrackerContext::startCycle() {
            m_currentTracker = m_rootTracker.get();
            m_runState = Executing;
        }
        void TrackerContext::completeCycle() { m_runState = CompletedCycle; }

        bool TrackerContext::completedCycle() const {
            return m_runState == CompletedCycle;
        }
        ITracker& TrackerContext::currentTracker() { return *m_currentTracker; }
        void TrackerContext::setCurrentTracker( ITracker* tracker ) {
            m_currentTracker = tracker;
        }

        TrackerBase::TrackerBase( NameAndLocation const& nameAndLocation,
                                  TrackerContext& ctx,
                                  ITracker* parent ):
            ITracker( nameAndLocation, parent ), m_ctx( ctx ) {}

        bool TrackerBase::isComplete() const {
            return m_runState == CompletedSuccessfully || m_runState == Failed;
        }

        void TrackerBase::open() {
            m_runState = Executing;
            moveToThis();
            if ( m_parent )
                m_parent->openChild();
        }

        void TrackerBase::close() {

            // Close any still open children (e.g. generators)
            while ( &m_ctx.currentTracker() != this )
                m_ctx.currentTracker().close();

            switch ( m_runState ) {
            case NeedsAnotherRun:
                break;

            case Executing:
                m_runState = CompletedSuccessfully;
                break;
            case ExecutingChildren:
                if ( std::all_of( m_children.begin(),
                                  m_children.end(),
                                  []( ITrackerPtr const& t ) {
                                      return t->isComplete();
                                  } ) )
                    m_runState = CompletedSuccessfully;
                break;

            case NotStarted:
            case CompletedSuccessfully:
            case Failed:
                CATCH_INTERNAL_ERROR( "Illogical state: " << m_runState );

            default:
                CATCH_INTERNAL_ERROR( "Unknown state: " << m_runState );
            }
            moveToParent();
            m_ctx.completeCycle();
        }
        void TrackerBase::fail() {
            m_runState = Failed;
            if ( m_parent )
                m_parent->markAsNeedingAnotherRun();
            moveToParent();
            m_ctx.completeCycle();
        }

        void TrackerBase::moveToParent() {
            assert( m_parent );
            m_ctx.setCurrentTracker( m_parent );
        }
        void TrackerBase::moveToThis() { m_ctx.setCurrentTracker( this ); }

        SectionTracker::SectionTracker( NameAndLocation const& nameAndLocation,
                                        TrackerContext& ctx,
                                        ITracker* parent ):
            TrackerBase( nameAndLocation, ctx, parent ),
            m_trimmed_name( trim( nameAndLocation.name ) ) {
            if ( parent ) {
                while ( !parent->isSectionTracker() )
                    parent = parent->parent();

                SectionTracker& parentSection =
                    static_cast<SectionTracker&>( *parent );
                addNextFilters( parentSection.m_filters );
            }
        }

        bool SectionTracker::isComplete() const {
            bool complete = true;

            if ( m_filters.empty() || m_filters[0].empty() ||
                 std::find( m_filters.begin(),
                            m_filters.end(),
                            m_trimmed_name ) != m_filters.end() ) {
                complete = TrackerBase::isComplete();
            }
            return complete;
        }

        bool SectionTracker::isSectionTracker() const { return true; }

        SectionTracker&
        SectionTracker::acquire( TrackerContext& ctx,
                                 NameAndLocation const& nameAndLocation ) {
            SectionTracker* section;

            ITracker& currentTracker = ctx.currentTracker();
            if ( ITracker* childTracker =
                     currentTracker.findChild( nameAndLocation ) ) {
                assert( childTracker );
                assert( childTracker->isSectionTracker() );
                section = static_cast<SectionTracker*>( childTracker );
            } else {
                auto newSection = Catch::Detail::make_unique<SectionTracker>(
                    nameAndLocation, ctx, &currentTracker );
                section = newSection.get();
                currentTracker.addChild( CATCH_MOVE( newSection ) );
            }
            if ( !ctx.completedCycle() )
                section->tryOpen();
            return *section;
        }

        void SectionTracker::tryOpen() {
            if ( !isComplete() )
                open();
        }

        void SectionTracker::addInitialFilters(
            std::vector<std::string> const& filters ) {
            if ( !filters.empty() ) {
                m_filters.reserve( m_filters.size() + filters.size() + 2 );
                m_filters.emplace_back(
                    StringRef{} ); // Root - should never be consulted
                m_filters.emplace_back(
                    StringRef{} ); // Test Case - not a section filter
                m_filters.insert(
                    m_filters.end(), filters.begin(), filters.end() );
            }
        }
        void SectionTracker::addNextFilters(
            std::vector<StringRef> const& filters ) {
            if ( filters.size() > 1 )
                m_filters.insert(
                    m_filters.end(), filters.begin() + 1, filters.end() );
        }

        std::vector<StringRef> const& SectionTracker::getFilters() const {
            return m_filters;
        }

        StringRef SectionTracker::trimmedName() const { return m_trimmed_name; }

    } // namespace TestCaseTracking

} // namespace Catch

#if defined( __clang__ )
#    pragma clang diagnostic pop
#endif
