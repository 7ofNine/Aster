#include "stdafx.h"
#include "CppUnitTest.h"
#include <stdexcept>
#include <cmath>
#include "TrigTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace {
    // constants for comparisons
    static const double C0 = 1.0;
    static const double S0 = 0.0;
    // Use Pi/2 for test
    static const double C1 =  cos(M_PI_2);  
    static const double C2 = -1.0;
    static const double C3 =  cos(3*M_PI_2);
    static const double C4 = 1.0;

    static const double S1 = sin(M_PI_2);
    static const double S2 = 0.0;
    static const double S3 = sin(3*M_PI_2);
    static const double S4 = 0.0;
    static const double S5 = 1.0;

    bool doubleEqual(const double v1, const double v2)
    {
        return fabs(v1 - v2) < 1.0e-15; // TODO make it dependent on the max number of digits
    }
}

namespace TestNewcomb
{		
	TEST_CLASS(NewcombTest)
	{
	public:
		
        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTable1)
            TEST_DESCRIPTION("Minimum of multiples shall be less equal than maximum of multiples")
        END_TEST_METHOD_ATTRIBUTE()
        // verify that min of multiples is samller than max of multiplse
		TEST_METHOD(testTrigTable1)
		{
            try {
                TrigTable(3, 1, 0.0); // min should be less equal than max
            }
            catch (std::out_of_range)
            {
                return; // this is o.k. an exception should have been thrown
            }

            Assert::Fail(L"Expected Exception, but not thrown");
		}


        // Single value instantiations
        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTable2)
            TEST_DESCRIPTION("Only multiple 0 defined") // this is a trivial case
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTable2)
        {
            TrigTable const trigTable(0, 0, M_PI_2); // min should be less equal than max

            Assert::AreEqual(C0, trigTable.cos(0), L"", LINE_INFO());
            Assert::AreEqual(S0, trigTable.sin(0), L"", LINE_INFO());

            try {
                    trigTable.cos(1);
                }
                catch (std::out_of_range)
                {
                    Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                    trigTable.sin(-1);
                }
                catch (std::out_of_range)
                {
                    Logger::WriteMessage("Out of range detected sin\n");
            }

        }


        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTable3)
            TEST_DESCRIPTION("Only multiple 1 defined")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTable3)
        {
            TrigTable const trigTable(1, 1, M_PI_2); 
            Assert::AreEqual(C1, trigTable.cos(1));
            Assert::AreEqual(S1, trigTable.sin(1));

            try {
                trigTable.cos(0);
            }
            catch (std::out_of_range)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(0);
            }
            catch (std::out_of_range)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTable4)
            TEST_DESCRIPTION("Only multiple 3 defined")
            END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTable4)
        {
            TrigTable const trigTable(3, 3, M_PI_2);

            Assert::IsTrue(doubleEqual(C3, trigTable.cos(3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S3, trigTable.sin(3)), L"", LINE_INFO());

            try {
                trigTable.cos(2);
            }
            catch (std::out_of_range)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(4);
            }
            catch (std::out_of_range)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }

        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTable5)
            TEST_DESCRIPTION("Only multiple -1 defined")
            END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTable5)
        {
            TrigTable const trigTable(-1, -1, M_PI_2);

            Assert::IsTrue(doubleEqual(-C1, trigTable.cos(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S1, trigTable.sin(-1)), L"", LINE_INFO());

            try {
                trigTable.cos(0);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(-2);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }



        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTable7)
            TEST_DESCRIPTION("Only multiple -3 defined")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTable7)
        {
            TrigTable const trigTable(-3, -3, M_PI_2);

            Assert::IsTrue(doubleEqual(-C3, trigTable.cos(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S3, trigTable.sin(-3)), L"", LINE_INFO());

            try {
                trigTable.cos(-2);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(-4);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }


        //Range instantiations
        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange1)
            TEST_DESCRIPTION("Range 0 - 1")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange1)
        {
            TrigTable const trigTable(0, 1, M_PI_2);

            Assert::IsTrue(doubleEqual(C0, trigTable.cos(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S0, trigTable.sin(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S1, trigTable.sin(1)), L"", LINE_INFO());


            try {
                trigTable.cos(-1);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(2);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }

        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange2)
            TEST_DESCRIPTION("Range 0 - 3")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange2)
        {
                TrigTable const trigTable(0, 3, M_PI_2);

                Assert::IsTrue(doubleEqual(C0, trigTable.cos(0)), L"", LINE_INFO());
                Assert::IsTrue(doubleEqual(S0, trigTable.sin(0)), L"", LINE_INFO());
                Assert::IsTrue(doubleEqual(C1, trigTable.cos(1)), L"", LINE_INFO());
                Assert::IsTrue(doubleEqual(S1, trigTable.sin(1)), L"", LINE_INFO());
                Assert::IsTrue(doubleEqual(C2, trigTable.cos(2)), L"", LINE_INFO());
                Assert::IsTrue(doubleEqual(S2, trigTable.sin(2)), L"", LINE_INFO());
                Assert::IsTrue(doubleEqual(C3, trigTable.cos(3)), L"", LINE_INFO());
                Assert::IsTrue(doubleEqual(S3, trigTable.sin(3)), L"", LINE_INFO());


                try {
                    trigTable.cos(-1);
                }
                catch (std::out_of_range & ex)
                {
                    Logger::WriteMessage("Out of range detected cos\n");
                }

                try {
                    trigTable.sin(4);
                }
                catch (std::out_of_range & ex)
                {
                    Logger::WriteMessage("Out of range detected sin\n");
                }

        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange3)
            TEST_DESCRIPTION("Range 2 - 4")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange3)
        {
            TrigTable const trigTable(2, 4, M_PI_2);

            Assert::IsTrue(doubleEqual(C2, trigTable.cos(2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S2, trigTable.sin(2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C3, trigTable.cos(3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S3, trigTable.sin(3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C4, trigTable.cos(4)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S4, trigTable.sin(4)), L"", LINE_INFO());


            try {
                trigTable.cos(1);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(5);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange4)
            TEST_DESCRIPTION("Range -1 - 3")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange4)
        {
            TrigTable const trigTable(-1, 3, M_PI_2);

            Assert::IsTrue(doubleEqual(C1,  trigTable.cos(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S1, trigTable.sin(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C0, trigTable.cos(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S0, trigTable.sin(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S1, trigTable.sin(1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C2, trigTable.cos(2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S2, trigTable.sin(2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C3, trigTable.cos(3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S3, trigTable.sin(3)), L"", LINE_INFO());

            try {
                trigTable.cos(-2);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(4);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange5)
            TEST_DESCRIPTION("Range -3 - 3")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange5)
        {
            TrigTable const trigTable(-3, 3, M_PI_2);

            Assert::IsTrue(doubleEqual(C3, trigTable.cos(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S3, trigTable.sin(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C2, trigTable.cos(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S2, trigTable.sin(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S1, trigTable.sin(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C0, trigTable.cos(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S0, trigTable.sin(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S1, trigTable.sin(1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C2, trigTable.cos(2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S2, trigTable.sin(2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C3, trigTable.cos(3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S3, trigTable.sin(3)), L"", LINE_INFO());

            try {
                trigTable.cos(-4);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(4);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange6)
            TEST_DESCRIPTION("Range -3 - 1")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange6)
        {
            TrigTable const trigTable(-3, 1, M_PI_2);

            Assert::IsTrue(doubleEqual(C3, trigTable.cos(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S3, trigTable.sin(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C2, trigTable.cos(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S2, trigTable.sin(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S1, trigTable.sin(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C0, trigTable.cos(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S0, trigTable.sin(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S1, trigTable.sin(1)), L"", LINE_INFO());

            try {
                trigTable.cos(-4);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(2);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange7)
            TEST_DESCRIPTION("Range -3 - 0")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange7)
        {
            TrigTable const trigTable(-3, 0, M_PI_2);

            Assert::IsTrue(doubleEqual(C3, trigTable.cos(-3)), L"", LINE_INFO());
            Assert::AreEqual(-S3, trigTable.sin(-3));
            Assert::IsTrue(doubleEqual(-S3, trigTable.sin(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C2, trigTable.cos(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S2, trigTable.sin(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S1, trigTable.sin(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C0, trigTable.cos(0)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(S0, trigTable.sin(0)), L"", LINE_INFO());

            try {
                trigTable.cos(-4);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(1);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange8)
            TEST_DESCRIPTION("Range -3 - -1")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange8)
        {
            TrigTable const trigTable(-3, -1, M_PI_2);

            Assert::IsTrue(doubleEqual(C3, trigTable.cos(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S3, trigTable.sin(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C2, trigTable.cos(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S2, trigTable.sin(-2)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(C1, trigTable.cos(-1)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S1, trigTable.sin(-1)), L"", LINE_INFO());

            try {
                trigTable.cos(-4);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(0);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(testTrigTableRange9)
            TEST_DESCRIPTION("Range -5 - -3")
        END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(testTrigTableRange9)
        {
            TrigTable const trigTable(-4, -3, M_PI_2);

            Assert::IsTrue(doubleEqual(C4, trigTable.cos(-4)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S4, trigTable.sin(-4)), L"", LINE_INFO());

            Assert::IsTrue(doubleEqual(C3, trigTable.cos(-3)), L"", LINE_INFO());
            Assert::IsTrue(doubleEqual(-S3, trigTable.sin(-3)), L"", LINE_INFO());

            try {
                trigTable.cos(-5);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected cos\n");
            }

            try {
                trigTable.sin(-2);
            }
            catch (std::out_of_range & ex)
            {
                Logger::WriteMessage("Out of range detected sin\n");
            }
        }

	};
}