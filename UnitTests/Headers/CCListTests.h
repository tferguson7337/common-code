#pragma once

#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCList.h>

// Test Helper Utils
#include <TestHelpers.hpp>


namespace CC
{
    class ListTests
    {
        // Type aliases.
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR()>;
        using UTList = std::list<UTFunc>;

        ListTests() = delete;
        ListTests(const ListTests&) = delete;
        ListTests(ListTests&&) = delete;
        ~ListTests() = delete;
        ListTests& operator=(const ListTests&) = delete;
        ListTests& operator=(ListTests&&) = delete;

    private:

        /// Test Subclasses \\\

        // Tests constructor behavior.
        class ConstructorTests;

        // Tests op-overloads and other methods related to copying.
        class CopyTests;

        // Tests op-overloads and other methods related to moving.
        class MoveTests;

        // Tests assignment methods.
        class AssignTests;

        // Tests insertion methods (e.g., Append, Prepend, InsertAt, etc.).
        class InsertionTests;

        // Tests getter methods (e.g., Front, Back, At, etc.).
        class GetterTests;

        // Tests comparison methods.
        class ComparisonTests;

        // Tests removal methods (e.g., PopFront, PopBack, RemoveAt, etc.).
        class RemovalTests;

    public:

        // Returns list of List unit tests.
        [[nodiscard]] static UTList GetTests();
    };


    // Tests constructor behavior.
    class ListTests::ConstructorTests
    {
    private:



    public:


    };


    // Tests op-overloads and other methods related to copying.
    class ListTests::CopyTests
    {

    };


    // Tests op-overloads and other methods related to moving.
    class ListTests::MoveTests
    {

    };


    // Tests assignment methods.
    class ListTests::AssignTests
    {

    };


    // Tests insertion methods (e.g., Append, Prepend, InsertAt, etc.).
    class ListTests::InsertionTests
    {

    };


    // Tests getter methods (e.g., Front, Back, At, etc.).
    class ListTests::GetterTests
    {

    };


    // Tests comparison methods.
    class ListTests::ComparisonTests
    {

    };


    // Tests removal methods (e.g., PopFront, PopBack, RemoveAt, etc.).
    class ListTests::RemovalTests
    {

    };

}