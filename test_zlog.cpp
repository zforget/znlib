#include "zlog.hpp"

int main(int argc, char *argv[])
{

    Log4z logger;
    logger.WriteLog("DEBUG: %d\n", __LINE__);
    logger << std::string("hahah");
    logger << "asdfasdfhahah" << "123791827";
    logger.setLogPath("test_zlog.log");
    logger.WriteLog("DEBUG: %d\n", __LINE__);
    logger.WriteLog("DEBUG: %d\n", __LINE__);
    
    return 0;
}
