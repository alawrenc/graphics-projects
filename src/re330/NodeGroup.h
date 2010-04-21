#ifndef __NodeGroup_h__
#define __NodeGroup_h__

#include "Node.h"
#include "RenderContext.h"
#include <list>

namespace RE330
{
    class NodeGroup : public Node
    {
    public:
        NodeGroup() {}
        ~NodeGroup()
            {
                end = children.end();
                for (it = children.begin(); it != end; it ++)
                {
                    delete *it;
                }

            }

        // return copy of list to prevent modification
        std::list<Node*> getChildren()
            {
                std::list<Node*> copy = std::list<Node*>(children);
                return copy;
            }

        // NOT IMPLEMENTED
        Node * getChild(int index)
            {
                it = children.begin();
                it = it++;// index;
                return *it;
            }

        void addChildNode(Node * child)
            {
                children.push_back(child);
                child->setParent(this);
            }

        void removeChild(Node * child)
            {
                children.remove(child);
            }

        int getNumChildren()
            {
                return children.size();
            }

    protected:
        std::list<Node*> children;
        std::list<Node*>::iterator it;
        std::list<Node*>::iterator end;
    };
}
#endif
