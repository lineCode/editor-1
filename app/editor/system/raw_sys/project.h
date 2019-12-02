#pragma once

#include "../../include.h"
#include "gl_object.h"
#include "component.h"
#include "res.h"
#include "raw.h"

class Project {
public:
    Project();
    bool IsOpen() const { return _root != nullptr; }
    const std::string & GetURL() const { return _url; }
    const std::string & GetDIR() const { return tools::GetFileFolder(_url); }
    
    void New(const std::string & url);
    void Load(const std::string & url);
    void Save(const std::string & url);

    const SharePtr<GLObject> & GetRoot()          { return _root; }
    const SharePtr<GLObject> & GetObject(uint id) { return _objects.at(id); }

    SharePtr<GLObject> NewObject();
    void DeleteObject(const uint &id);
    void DeleteObject(const SharePtr<GLObject> & object);

private:
    std::map<uint, SharePtr<GLObject>> _objects;
    SharePtr<GLObject> _root;
    std::string         _url;
    uint                _acc;
};