#include "sys_config.h"
#include <filesystem>

namespace fs = std::filesystem;

void SysConfig::read_all_config()
{
    read_ftps_config();
    read_configured_points();
    read_system_config();
}


void SysConfig::read_ftps_config()
{

}

void SysConfig::read_configured_points()
{

}

void SysConfig::read_system_config()
{

}

