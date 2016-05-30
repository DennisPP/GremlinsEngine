#include "ColladaAsset.h"

#include "tinyxml.h"

TColladaAsset::TColladaAsset()
{
}

void TColladaAsset::Parse(TiXmlElement *xml)
{
    TiXmlElement* up = xml->FirstChildElement("up_axis");
    if( up )
    {
        const char* text = up->GetText();
        if( text )
        {
            if( 0==strcmp(text, "X_UP"))
                upAxis = TUpAxisType::X_UP;
            else if( 0==strcmp(text, "Y_UP"))
                upAxis = TUpAxisType::Y_UP;
            else if( 0==strcmp(text, "Z_UP"))
                upAxis = TUpAxisType::Z_UP;
            
        }
    }
}