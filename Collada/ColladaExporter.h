#ifndef __ColladaExporter__
#define __ColladaExporter__

class TColladaModel;

class TColladaExporter
{
public:
    virtual bool ExportMesh(TColladaModel* model, const char* path) = 0;
    virtual bool ExportAnimation(TColladaModel* model, const char* path) = 0;
};


#endif