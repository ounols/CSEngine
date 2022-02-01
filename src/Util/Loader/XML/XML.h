#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

#define XML_EOF 0
#define XML_NODE 1
#define XML_VALUE 2

class XValue : public std::string {
public:
    XValue();

    XValue(const std::string& str);

    XValue& operator=(const std::string& str);

    std::vector<float> toFloatVector() const;

    std::vector<int> toIntegerVector() const;

    std::vector<std::string> toStringVector() const;
};

class XAttrib {
public:
    std::string name;
    std::string value;

    XAttrib();

    XAttrib(const std::string& str);

    XAttrib(const char* _name, const char* _value);

    std::string toString() const;
};

class XNode {
private:
    const XNode* _getNode(const char* node_name) const;

    const XNode* _getNodeByAttribute(const char* node_name, const char* attrib_name, const char* attrib_value) const;

public:
    XNode* parent;

    std::string name;
    XValue value;
    std::string raw;
    std::vector<XAttrib> attributes;
    std::vector<XNode> children;

    int sub_index = -1;

    XNode();

    void print() const; // print the string representation in the format: name: value ...attributes...
    const XNode& getNode(const char* name) const;

    const XNode& getChild(const char* name) const;

    bool hasChild(const char* name) const;

    const XAttrib& getAttribute(const char* name) const;

    bool hasAttribute(const char* name) const;

    const XNode& getNodeByAttribute(const char* node_name, const char* attrib_name, const char* attrib_value) const;
    bool hasNodeByAttribute(const char* node_name, const char* attrib_name, const char* attrib_value) const;
};

class XFILE {
private:
//    FILE* file; // the file being read
    std::string file;
    int file_index = 0;

    int read(std::string& buffer);


public:
    XFILE(){}
    XFILE(const char* str); // read the file into the node heirchy
    const XNode* getRoot();
    const XNode* loadBuffer(std::string buffer);

    ~XFILE();
};
