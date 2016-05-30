#include "GremlinsFramework.h"

#include <ctime>

TGremlinsFramework* TGremlinsFramework::instance = NULL;

TGremlinsFramework* TGremlinsFramework::GetInstance()
{
    if( ! instance )
        instance = new TGremlinsFramework();
    return instance;
}

const std::string& TGremlinsFramework::GetAssetRoot() const
{
    return assetRoot;
}

void TGremlinsFramework::SetAssetRoot(const std::string& root)
{
    assetRoot = root;
}

const std::string& TGremlinsFramework::GetDocumentRoot() const
{
    return documentRoot;
}

void TGremlinsFramework::SetDocumentRoot(const std::string& root)
{
    documentRoot = root;
    printf("Ducument root set to :%s\n", root.c_str());
}

const std::string& TGremlinsFramework::GetEngineAssetRoot() const
{
    return engineAssetRoot;
}

void TGremlinsFramework::SetEngineAssetRoot(const std::string& root)
{
    engineAssetRoot = root;
}

const std::string& TGremlinsFramework::GetDevice() const
{
    return device;
}

void TGremlinsFramework::SetDevice(const std::string& _device)
{
    device = _device;

    printf("Gremlins::::Device Set to:%s\n", device.c_str());
}


std::string TGremlinsFramework::GetDateTimeString()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[256];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,256,"%Y-%m-%d %I:%M:%S",timeinfo);
    std::string str(buffer);

    return str;
}