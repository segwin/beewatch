//==============================================================================
// Courtesy of JohnnyHendriks:
// https://github.com/JohnnyHendriks/TestAdapter_Catch2
//==============================================================================

#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

void Discover(Catch::Session& session);

int main(int argc, char* argv[])
{
    Catch::Session session;

    bool doDiscover = false;

    // Add option to commandline
    {
        using namespace Catch::clara;

        auto cli = session.cli()
            | Opt(doDiscover)
            ["--discover"]
        ("Perform VS Test Adaptor discovery");

        session.cli(cli);
    }

    // Process commandline
    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0) return returnCode;

    // Check if custom discovery needs to be performed
    if (doDiscover)
    {
        try
        {
            Discover(session);
            return 0;
        }
        catch (std::exception& ex)
        {
            Catch::cerr() << ex.what() << std::endl;
            return Catch::MaxExitCode;
        }
    }

    // Let Catch2 do its thing
    return session.run();
}

void Discover(Catch::Session& session)
{
    using namespace Catch;

    // Retrieve testcases
    const auto& config = session.config();
    auto testspec = config.testSpec();
    auto testcases = filterTests(Catch::getAllTestCasesSorted(config)
        , testspec
        , config);

    // Setup reporter
    TestRunInfo runInfo(config.name());

    auto pConfig = std::make_shared<Config const>(session.configData());
    auto reporter = getRegistryHub()
        .getReporterRegistry()
        .create("xml", pConfig);

    Catch::Totals totals;

    reporter->testRunStarting(runInfo);
    reporter->testGroupStarting(GroupInfo(config.name(), 1, 1));

    // Report test cases
    for (const auto& testcase : testcases)
    {
        Catch::TestCaseInfo caseinfo(testcase.name
            , testcase.className
            , testcase.description
            , testcase.tags
            , testcase.lineInfo);
        reporter->testCaseStarting(caseinfo);
        reporter->testCaseEnded(Catch::TestCaseStats(caseinfo
            , totals
            , ""
            , ""
            , false));
    }

    reporter->testGroupEnded(Catch::GroupInfo(config.name(), 1, 1));
    TestRunStats testrunstats(runInfo, totals, false);
    reporter->testRunEnded(testrunstats);
}