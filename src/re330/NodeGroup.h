#ifndef __NodeGroup_h__
#define __NodeGroup_h__

#include "Node.h"
#include "GLRenderContext.h"
#include <list>

namespace RE330
{
    class NodeGroup : public Node
    {
    public:
        NodeGroup();

        virtual void draw(Matrix4 m, GLRenderContext rc, Camera c) = 0;

        // return copy of list to prevent modification
        std::list getChildren()
            {
                std::list<Node> copy = std::list<Node>(children);
                return copy;
            }

        Node getChild(int index)
            {
                it = children.begin();
                i += index;
                return *i;
            }

        void addChildNode(Node child)
            {
                children.push_back(child);
                child.setParent(&this);
                child.updateMatrix();
            }

        Node removeChild(int i)
            {
                children.erase(i);
            }

        Node removeChild(Node child)
            {
                children.remove(child);
            }

        int getNumChildren()
            {
                return children.size();
            }

    protected:
        std::list<Node> children;
        std::list<Node>::iterator it;
    }
}
#endif
