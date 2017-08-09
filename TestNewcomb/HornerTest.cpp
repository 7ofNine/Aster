#include "stdafx.h"
#include "CppUnitTest.h"
#include <stdexcept>
#include <cmath>
#include "Horner.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace TestNewcomb
{
    TEST_CLASS(HornerTest)
    {
    public:
        BEGIN_TEST_METHOD_ATTRIBUTE(testHorner1)
            TEST_DESCRIPTION("Test constant")
       END_TEST_METHOD_ATTRIBUTE()
       // verify that is is a constant
       TEST_METHOD(testHorner1)
        {
            Horner poly = { 1.0 };

            Assert::AreEqual(1.0, poly(0), L"", LINE_INFO());
            Assert::AreEqual(1.0, poly(99), L"", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testHorner2)
            TEST_DESCRIPTION("Test linear")
            END_TEST_METHOD_ATTRIBUTE()
            // verify that is is a constant
            TEST_METHOD(testHorner2)
        {
            Horner poly = { 1.0, 2.0 };

            Assert::AreEqual(2.0 * 0.0  + 1.0, poly(0.0), L"", LINE_INFO());
            Assert::AreEqual(2.0 * 99.0 + 1.0, poly(99.0), L"", LINE_INFO());
            Assert::AreEqual(2.0 * (-99.0) + 1.0, poly(-99.0), L"", LINE_INFO());
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testHorner3)
            TEST_DESCRIPTION("Test quadraticr")
            END_TEST_METHOD_ATTRIBUTE()
            // verify that is is a constant
            TEST_METHOD(testHorner3)
        {
            Horner poly = { 1.0, 2.0, 1.0  };

            Assert::AreEqual(1.0 * 0.0  * 0.0  + 2.0 * 0.0  + 1.0,       poly(0.0), L"", LINE_INFO());
            Assert::AreEqual(1.0 * (-4.0) * (-4.0) + 2.0 * (-4.0) + 1.0, poly(-4.0), L"", LINE_INFO());
            Assert::AreEqual(1.0 * 99.0 * 99.0 + 2.0 * 99.0 + 1.0,       poly(99.0), L"", LINE_INFO());
        }
    };

}