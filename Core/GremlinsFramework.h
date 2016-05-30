#ifndef __GremlinsFramework__
#define __GremlinsFramework__

#define SAVE_DELETE(x)  if(x) { delete x; x = NULL; }
#define SAVE_ARRAY_DELETE(x)  if(x) { delete [] x; x = NULL; }


#include <string>

class TGremlinsFramework
{
    public:
    static TGremlinsFramework* GetInstance();
    const std::string& GetAssetRoot() const;
    void SetAssetRoot(const std::string& root);
    
    const std::string& GetDocumentRoot() const;
    void SetDocumentRoot(const std::string& root);
    
    
    const std::string& GetEngineAssetRoot() const;
    void SetEngineAssetRoot(const std::string& root);
    
    const std::string& GetDevice() const;
    void SetDevice(const std::string& device);
    
    
    std::string GetDateTimeString();
    
protected:
    TGremlinsFramework(){};
    std::string assetRoot;
    std::string engineAssetRoot;
    std::string documentRoot;
    std::string device;
    
    static TGremlinsFramework* instance;
};

#endif