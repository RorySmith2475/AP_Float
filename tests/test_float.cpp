// #define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include "AP/Float.hpp"

using namespace AP;

TEST_CASE("Constructors")
{
    SECTION("Float input")
    {
        auto i = GENERATE(range(-1.0e37f, 1.0e37f, 1.23456789e34f));

        Float f(i);

        INFO("Input: " << i);
        INFO("Result: " << f.toFloat());
        INFO("Actual: " << i);
        REQUIRE(f.toFloat() == Approx(i));
    }

    SECTION("Double input")
    {
        auto i = GENERATE(range(-1.0e200, 1.0e200, 1.23456789e197));

        Float f(i);

        INFO("Input: " << i);
        INFO("Result: " << f.toDouble());
        INFO("Actual: " << i);
        REQUIRE(f.toDouble() == Approx(i));
    }

    SECTION("Integer input")
    {
        auto i = GENERATE(-55, 55U, 2000UL, -4L);

        Float f(i);

        INFO("Input: " << i);
        INFO("Result: " << f.toDouble());
        INFO("Actual: " << i);
        REQUIRE(f.toDouble() == Approx(static_cast<double>(i)));
    }
    
    SECTION("Invalid Input")
    {
        Float f;
        f = Float("afsdjklnasdfnjklasdfjknl");
        REQUIRE(f.getState() == Float::ERROR);
        f = Float("123.456e-e");
        REQUIRE(f.getState() == Float::ERROR);
        f = Float("123 .3");
        REQUIRE(f.getState() == Float::ERROR);
        f = Float("1 23.4");
        REQUIRE(f.getState() == Float::ERROR);
        f = Float("1 2");
        REQUIRE(f.getState() == Float::ERROR);
        f = Float("567.4 e -7");
        REQUIRE(f.getState() == Float::ERROR);
    }
}

TEST_CASE("Comparisons")
{
    REQUIRE(Float("1.9") < Float("2.34"));
    REQUIRE(Float("1.23") < Float("20000000000.34"));
    REQUIRE(Float("-1000000.23") < Float("2.34"));
    REQUIRE(Float("-1000000.23") < Float("-10.0"));
    REQUIRE(Float("12341241234123412341234123412341234.23412341234") < Float("8975432789052347809254324538907254387902453789054238790542387904523879045238790254387905243879078924350.45378904523987045239780542387902453789023457890"));
    

    REQUIRE(Float("1.23") > Float("0.1"));
    REQUIRE(Float("1.2000000300000") > Float("0.00000000001"));
    REQUIRE(Float("0.00000000001") > Float("-1.2000000300000"));
    REQUIRE(Float("0.00002") > Float("0.0000000000003"));

    REQUIRE(Float("1.23") == Float("1.23"));
}

TEST_CASE("Addition")
{
    SECTION("Edge cases")
    {
        auto i = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
        SECTION("")
        {
            auto j = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());

            auto result = Float(i) + Float(j);

            INFO("i: " << i);
            INFO("j: " << j);
            INFO("result: " << result);
            if(std::isnan(i + j))
            {
                REQUIRE(result.getState() == Float::ERROR);
            }
            else
            {
                REQUIRE(result.getState() != Float::ERROR);
                REQUIRE(result.toFloat() == Approx(i + j));
            }
        }
    }

    SECTION("Random values")
    {
        // auto i = GENERATE(range(-1.0e15f, 1.0e15f, 1.23456789123456789e12f));
        auto i = GENERATE(range(-1.0e36f, 1.0e36f, 1.23456789123456789e35f));
        SECTION("")
        {
            auto j = GENERATE(range(-1.0e36f, 1.0e36f, 1.23456789123456789e35f));

            Float f1(i);
            Float f2(j);

            INFO("f1: " << f1);
            INFO("f2: " << f2);
            REQUIRE(f1.getState() == Float::NORMAL);
            REQUIRE(f2.getState() == Float::NORMAL);
            REQUIRE(f1.toFloat() == Approx(i));
            REQUIRE(f2.toFloat() == Approx(j));

            f1 += f2;
            INFO("result: " << f1);
            INFO("wanted: " << Float(i + j));
            REQUIRE(f1.toFloat() == Approx(i + j));
        }
    }    
}

TEST_CASE("Subtraction")
{
    SECTION("Edge cases")
    {
        auto i = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
        SECTION("")
        {
            auto j = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            auto result = Float(i) - Float(j);
            
            INFO("i: " << i);
            INFO("j: " << j);
            INFO("result: " << result);
            if(std::isnan(i - j))
            {
                REQUIRE(result.getState() == Float::ERROR);
            }
            else
            {
                REQUIRE(result.getState() != Float::ERROR);
                REQUIRE(result.toFloat() == Approx(i - j));
            }
        }
    }

    SECTION("Random values")
    {
        auto i = GENERATE(range(-1.0e36f, 1.0e36f, 1.23456789123456789e35f));
        SECTION("")
        {
            auto j = GENERATE(range(-1.0e36f, 1.0e36f, 1.23456789123456789e35f));

            Float f1(i);
            Float f2(j);

            INFO("f1: " << f1);
            INFO("f2: " << f2);
            REQUIRE(f1.getState() == Float::NORMAL);
            REQUIRE(f2.getState() == Float::NORMAL);
            REQUIRE(f1.toFloat() == Approx(i));
            REQUIRE(f2.toFloat() == Approx(j));

            f1 -= f2;
            INFO("result: " << f1);
            INFO("wanted: " << Float(i - j));
            REQUIRE(f1.toFloat() == Approx(i - j));
        }
    }
}

TEST_CASE("Multiplication")
{
    SECTION("Edge cases")
    {
        auto i = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
        SECTION("")
        {
            auto j = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            auto result = Float(i) * Float(j);
            
            INFO("i: " << i);
            INFO("j: " << j);
            INFO("result: " << result);
            if(std::isnan(i * j))
            {
                REQUIRE(result.getState() == Float::ERROR);
            }
            else
            {
                REQUIRE(result.getState() != Float::ERROR);
                REQUIRE(result.toFloat() == Approx(i * j));
            }
        }
    }

    SECTION("Random values")
    {
        auto i = GENERATE(range(-1.0e18f, 1.0e18f, 1.23456789123456789e17f));
        SECTION("")
        {
            auto j = GENERATE(range(-1.0e18f, 1.0e18f, 1.23456789123456789e17f));

            Float f1(i);
            Float f2(j);

            INFO("f1: " << f1);
            INFO("f2: " << f2);
            REQUIRE(f1.getState() == Float::NORMAL);
            REQUIRE(f2.getState() == Float::NORMAL);
            REQUIRE(f1.toFloat() == Approx(i));
            REQUIRE(f2.toFloat() == Approx(j));

            f1 *= f2;
            INFO("result: " << f1);
            INFO("wanted: " << Float(i * j).toFloat() << std::scientific);
            REQUIRE(f1.toFloat() == Approx(i * j));
        }
    }
}

TEST_CASE("Division")
{
    SECTION("Edge cases")
    {
        auto i = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
        SECTION("")
        {
            auto j = GENERATE(0.0, -0.0, 1234.5678, -1234.5678, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            auto result = Float(i) / Float(j);
            
            INFO("i: " << i);
            INFO("j: " << j);
            INFO("result: " << result);
            if(std::isnan(i / j))
            {
                REQUIRE(result.getState() == Float::ERROR);
            }
            else
            {
                REQUIRE(result.getState() != Float::ERROR);
                REQUIRE(result.toFloat() == Approx(i / j));
            }
        }
    }

    SECTION("Random values")
    {
        auto i = GENERATE(range(-1.0e18f, 1.0e18f, 1.23456789123456789e17f));
        SECTION("")
        {
            auto j = GENERATE(range(-1.0e18f, 1.0e18f, 8.23456789123456789e17f));

            Float f1(i);
            Float f2(j);

            INFO("f1: " << f1);
            INFO("f2: " << f2);
            REQUIRE(f1.getState() == Float::NORMAL);
            REQUIRE(f2.getState() == Float::NORMAL);
            REQUIRE(f1.toFloat() == Approx(i));
            REQUIRE(f2.toFloat() == Approx(j));

            f1 /= f2;
            INFO("result: " << f1);
            INFO("wanted: " << Float(i / j).toFloat() << std::scientific);
            REQUIRE(f1.toFloat() == Approx(i / j));
        }
    }

#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
    SECTION("benchmark")
    {
    BENCHMARK_ADVANCED("sqrt")(Catch::Benchmark::Chronometer meter)
    {
        Float f1(1.23414321432132414321431243121342e280);
        meter.measure([&]{
            f1 = AP::sqrt(f1).value();
            return f1.toDouble();
        });
    };
    }
#endif
}
