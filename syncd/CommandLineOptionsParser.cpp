#include "CommandLineOptionsParser.h"

#include "meta/lai_serialize.h"

#include "swss/logger.h"

#include <getopt.h>

#include <iostream>

using namespace syncd;

std::shared_ptr<CommandLineOptions> CommandLineOptionsParser::parseCommandLine(
        _In_ int argc,
        _In_ char **argv)
{
    SWSS_LOG_ENTER();

    auto options = std::make_shared<CommandLineOptions>();

#ifdef LAITHRIFT
    const char* const optstring = "dp:f:g:x:UCsz:lr:h";
#else
    const char* const optstring = "dp:f:g:x:UCsz:lh";
#endif // LAITHRIFT

    while (true)
    {
        static struct option long_options[] =
        {
            { "diag",                    no_argument,       0, 'd' },
            { "profile",                 required_argument, 0, 'p' },
            { "enableUnittests",         no_argument,       0, 'U' },
            { "enableConsistencyCheck",  no_argument,       0, 'C' },
            { "syncMode",                no_argument,       0, 's' },
            { "redisCommunicationMode",  required_argument, 0, 'z' },
            { "enableLaiBulkSupport",    no_argument,       0, 'l' },
            { "globalContext",           required_argument, 0, 'g' },
            { "contextContig",           required_argument, 0, 'x' },
#ifdef LAITHRIFT
            { "rpcserver",               no_argument,       0, 'r' },
#endif // LAITHRIFT
            { "help",                    no_argument,       0, 'h' },
            { "fordebug",                no_argument,       0, 'f' },
            { 0,                         0,                 0,  0  }
        };

        int option_index = 0;

        int c = getopt_long(argc, argv, optstring, long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'd':
                options->m_enableDiagShell = true;
                break;

            case 'p':
                options->m_profileMapFile = std::string(optarg);
                break;

            case 'U':
                options->m_enableUnittests = true;
                break;

            case 'C':
                options->m_enableConsistencyCheck = true;
                break;

            case 's':
                SWSS_LOG_WARN("param -s is depreacated, use -z");
                options->m_enableSyncMode = true;
                break;

            case 'z':
                lai_deserialize_redis_communication_mode(optarg, options->m_redisCommunicationMode);
                break;

            case 'l':
                options->m_enableLaiBulkSupport = true;
                break;

            case 'g':
                options->m_globalContext = (uint32_t)std::stoul(optarg);
                break;

            case 'x':
                options->m_contextConfig = std::string(optarg);
                break;

            case 'f':
                options->m_loglevel = (uint32_t)std::stoul(optarg);
                break;

#ifdef LAITHRIFT
            case 'r':
                options->m_runRPCServer = true;
                break;
#endif // LAITHRIFT

            case 'h':
                printUsage();
                exit(EXIT_SUCCESS);

            case '?':
                SWSS_LOG_WARN("unknown option %c", optopt);
                printUsage();
                exit(EXIT_FAILURE);


            default:
                SWSS_LOG_ERROR("getopt_long failure");
                exit(EXIT_FAILURE);
        }
    }

    return options;
}

void CommandLineOptionsParser::printUsage()
{
    SWSS_LOG_ENTER();

#ifdef LAITHRIFT
    std::cout << "Usage: syncd [-d] [-p profile] [-U] [-C] [-s] [-z mode] [-l] [-g idx] [-x contextConfig] [-r] [-h]" << std::endl;
#else
    std::cout << "Usage: syncd [-d] [-p profile] [-U] [-C] [-s] [-z mode] [-l] [-g idx] [-x contextConfig] [-f fordebug] [-h]" << std::endl;
#endif // LAITHRIFT

    std::cout << "    -d --diag" << std::endl;
    std::cout << "        Enable diagnostic shell" << std::endl;
    std::cout << "    -p --profile profile" << std::endl;
    std::cout << "        Provide profile map file" << std::endl;
    std::cout << "    -U --enableUnittests" << std::endl;
    std::cout << "        Metadata enable unittests" << std::endl;
    std::cout << "    -C --enableConsistencyCheck" << std::endl;
    std::cout << "        Enable consisteny check DB vs ASIC after comparison logic" << std::endl;
    std::cout << "    -s --syncMode" << std::endl;
    std::cout << "        Enable synchronous mode (depreacated, use -z)" << std::endl;
    std::cout << "    -z --redisCommunicationMode" << std::endl;
    std::cout << "        Redis communication mode (redis_async|redis_sync), default: redis_async" << std::endl;
    std::cout << "    -l --enableBulk" << std::endl;
    std::cout << "        Enable LAI Bulk support" << std::endl;
    std::cout << "    -g --globalContext" << std::endl;
    std::cout << "        Global context index to load from context config file" << std::endl;
    std::cout << "    -x --contextConfig" << std::endl;
    std::cout << "        Context configuration file" << std::endl;
    std::cout << "    -f --fordebug" << std::endl;
    std::cout << "        debug level(0-EMERGE,1-ALERT,2-CRIT,3-ERROR,4-WARN,5-NOTICE,6-INFO,7-DEBUG)" << std::endl;

#ifdef LAITHRIFT

    std::cout << "    -r --rpcserver" << std::endl;
    std::cout << "        Enable rpcserver" << std::endl;

#endif // LAITHRIFT

    std::cout << "    -h --help" << std::endl;
    std::cout << "        Print out this message" << std::endl;
}
