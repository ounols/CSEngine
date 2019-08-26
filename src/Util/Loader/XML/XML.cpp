#include "../../AssetsDef.h"
#include "XML.h"

/************************************************************************************************************************************************/

XValue::XValue() {}

XValue::XValue(const std::string& str) : std::string(str) {}

XValue& XValue::operator=(const std::string& str) {
    this->assign(str);
    return *this;
}

std::vector<float> XValue::toFloatVector() const {
    std::string buffer = "";
    std::vector<float> array;

    for (unsigned int i = 0; i < this->length(); i++) {
        int acsii = (int) this->at(i);
        buffer += (char) acsii;

        if (acsii == 32 || i == this->length() - 1) {
            float f = atof(buffer.c_str());
            array.push_back(f);
            buffer.clear();
        }
    }

    return array;
}

std::vector<int> XValue::toIntegerVector() const {
    std::string buffer = "";
    std::vector<int> array;

    for (unsigned int i = 0; i < this->length(); i++) {
        int acsii = (int) this->at(i);
        buffer += (char) acsii;

        if (acsii == 32 || i == this->length() - 1) {
            float f = atoi(buffer.c_str());
            array.push_back(f);
            buffer.clear();
        }
    }

    return array;
}

std::vector<std::string> XValue::toStringVector() const {
    std::string buffer = "";
    std::vector<std::string> array;

    for (unsigned int i = 0; i < this->length(); i++) {
        int acsii = (int) this->at(i);
        buffer += (char) acsii;

        if (acsii == 32 || i == this->length() - 1) {
            array.push_back(buffer);
            buffer.clear();
        }
    }

    return array;
}

/************************************************************************************************************************************************/

XAttrib::XAttrib() {}

XAttrib::XAttrib(const std::string& str) {
    size_t index = str.find('=');
    name = str.substr(0, index); // copy the name from the indices: 0 to the equal to sign's index
    value = str.substr(index + 2, str.substr(index + 2).size() -
                                  1); // copy the value, skip the equal to sign and the 2 quotation marks
}

XAttrib::XAttrib(const char* _name, const char* _value) {
    this->name = std::string(_name);
    this->value = std::string(_value);
}

std::string XAttrib::toString() const {
    return (name + "=\"" + value + "\"");
}

/************************************************************************************************************************************************/

const XNode* XNode::_getNode(const char* node_name) const {
    if (strcmp(node_name, this->name.c_str()) == 0)
        return this;

    for (unsigned int i = 0; i < this->children.size(); i++) {
        const XNode* res = this->children.at(i)._getNode(node_name);
        if (res != NULL) return res;
    }

    return NULL;
}

const XNode*
XNode::_getNodeByAttribute(const char* node_name, const char* attrib_name, const char* attrib_value) const {
    if (node_name == NULL || strcmp(node_name, this->name.c_str()) == 0) {
        for (unsigned int i = 0; i < this->attributes.size(); i++) {
            if (attrib_name == NULL || strcmp(attrib_name, this->attributes.at(i).name.c_str()) == 0) {
                if (attrib_value == NULL || strcmp(attrib_value, this->attributes.at(i).value.c_str()) == 0) {
                    return this;
                }
            }
        }
    }

    for (unsigned int i = 0; i < this->children.size(); i++) {
        const XNode* res = this->children.at(i)._getNodeByAttribute(node_name, attrib_name, attrib_value);
        if (res != NULL) return res;
    }

    return NULL;
}

XNode::XNode() {
    this->parent = NULL;
    this->name = "";
    this->value = "";
    this->attributes = std::vector<XAttrib>();
    this->children = std::vector<XNode>();
}

void XNode::print() const // print the string representation in the format: name: value ...attributes...
{
    printf("%s: [%s] ", name.c_str(), value.c_str());
    for (unsigned int i = 0; i < this->attributes.size(); i++)
        printf("%s ", this->attributes.at(i).toString().c_str());
    printf("\n");
}

const XNode& XNode::getNode(const char* name) const {
    const XNode* res = this->_getNode(name);
    if (!res) {
        printf("Error: Node [%s] not found.", name);
        throw -1;
    }
    return *res;
}

const XAttrib& XNode::getAttribute(const char* name) const {
    for (unsigned int i = 0; i < this->attributes.size(); i++) {
        if (strcmp(this->attributes.at(i).name.c_str(), name) == 0) return this->attributes.at(i);
    }

    printf("Warning: Attribute [%s] not found.", name);
    throw -1;
}

const XNode& XNode::getNodeByAttribute(const char* node_name, const char* attrib_name, const char* attrib_value) const {
    const XNode* res = this->_getNodeByAttribute(node_name, attrib_name, attrib_value);
    if (!res) {
        printf("Warning: Node with ");
        if (node_name != NULL) printf("name \"%s\" and ", node_name);
        printf("attribute [");
        printf("%s=", attrib_name == NULL ? "\"\"" : attrib_name);
        printf("%s", attrib_value == NULL ? "\"\"" : attrib_value);
        printf("] not found.\n");
        throw -1;
    }
    return *res;
}

const XNode& XNode::getChild(const char* name) const {
    for (unsigned int i = 0; i < this->children.size(); i++) {
        if (strcmp(this->children[i].name.c_str(), name) == 0) return this->children[i];
    }

    printf("Error: Node [%s] not found.\n", name);
    throw -1;
}

/************************************************************************************************************************************************/

int XFILE::read(std::string& buffer) {
    buffer.clear();
    for (; file_index < file.length(); file_index++) {
        //file_index++;
        char c = file[file_index];
        if (c == '>') {
            file_index++; // get the next character to check what kind of a line this is

//            if (file_index >= file.length() && c != '\n') // move back if we have not reached the end of the file or line
//                file_index--;

            return XML_NODE; // this must be a node since this statement was ended by '>'
        } else if (c == '<' && buffer.length() > 0) {
            return XML_VALUE; // this must be a value since this statement was ended by  '<'
        } else if (c != '<') // check again if this isnt '<' since the last statement checks length as well
        {
            if (c == ' ' && buffer.length() == 0)
                continue;
            buffer += c;
        }
    }

    return XML_EOF; // if we have reached here then the file is done being read
}

XFILE::XFILE(const char* str) { // read the file into the node heirchy
    this->file = CSE::OpenAssetsTxtFile(str);
    if (file.empty()) {
#ifndef __ANDROID__
        printf("XML file [%s] not found!\n", str);
#endif
        throw -1;
    }
}

const XNode* XFILE::getRoot() {
    XNode* root = new XNode();
    XNode* current = root;

    std::string buffer;
    int ret = 0;
    while (ret = read(buffer), ret != XML_EOF) // read the next node/value
    {
        if (buffer[0] == '?') // comment
            continue;

        else if (buffer[0] == '/') // end of node
        {
            if (buffer.substr(1) != current->name) // confirm the node names match
                printf("[WARNING]: Name mismatch: [%s] vs [%s]\n", buffer.substr(1).c_str(), current->name.c_str());
            current = current->parent;
        } else if (ret == XML_VALUE) // if this is a value string, we simply set it to the current node
            current->value = buffer;

        else if (buffer.find(' ') == std::string::npos) // this node has no attributes, simply append it to the children
        {
            XNode node = XNode();
            node.name = buffer;
            node.parent = current;
            current->children.push_back(node);

            if (buffer.back() != '/') // this  node does not instant terminate, set it to the current node
                current = &current->children.back();
            else // this node does instant terminate, make sure you remove the '/' at the end
                current->children.back().name.pop_back();
        } else // this node does have attributes
        {
            char* str = (char*) malloc(buffer.length() + 1); // allocate and initalize the buffer
            if (!str) throw -1;
            strcpy(str, buffer.c_str());

            char* token = strtok(str, " "); // start tokenizing
            XNode node = XNode();
            node.name = token; // the first token is the name
            node.parent = current;

            while (true) // parse the attribute tokens
            {
                token = strtok(NULL, " "); // get the next token
                if (token == NULL)
                    break;

                if (token[strlen(token) - 1] == '/') // if the current token ends with a '/', then remove it
                    token[strlen(token) - 1] = '\0';

                XAttrib attribute = XAttrib(std::string(token));
                node.attributes.push_back(attribute);
            }

            current->children.push_back(node);
            if (buffer.back() != '/') // if this node does not instant-terminate, set it to the current node
                current = &current->children.back();
            free(str);
        }
    }

    return root;
}

XFILE::~XFILE() {
//    fclose(this->file);
}
