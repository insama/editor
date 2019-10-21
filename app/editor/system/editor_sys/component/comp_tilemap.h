#pragma once

#include "component.h"
#include "../../raw_sys/raw_sys.h"
#include "../../interface/render.h"

class CompTilemap : public Component {
private:
    enum UpdateEnum { 
        kTilemap    = 1,
        kTrackPoint = 2,
    };

    struct Atlas {
        uint mSpace;
        uint mOffset;
        uint mIndexBase;
        SharePtr<GLTexture> mTexture;
    };

public:
    CompTilemap();
    virtual void OnAdd() override;
    virtual void OnDel() override;
    virtual void OnUpdate(UIObjectGLCanvas * canvas, float dt) override;

    virtual const std::string & GetName() override;
    virtual void EncodeBinary(std::ofstream & os) override;
    virtual void DecodeBinary(std::ifstream & is) override;
    virtual bool OnModifyProperty(const std::any & oldValue, 
                                  const std::any & newValue, 
                                  const std::string & title) override;

protected:
    virtual std::vector<Property> CollectProperty() override;

private:
    void Update();
    void UpdateTilemap();
    void UpdateVertexs(uint mapW, uint mapH, 
                       uint tileW, uint tileH,
                       const mmc::Json::Pointer & data,
                       const std::vector<Atlas> & atlass,
                       std::vector<uint>           & indexs,
                       std::vector<GLMesh::Vertex> & points);
    void UpdateAtlass(uint base, 
                      const std::string & url, 
                      std::vector<Atlas> & atlass);

    //glm::vec4 

private:
    std::string     _url;
    glm::vec2       _size;
    glm::vec2       _anchor;
    uint            _update;

    SharePtr<GLMesh>                    _mesh;
    std::vector<Atlas>                  _atlass;
    SharePtr<GLProgram>                 _program;
};