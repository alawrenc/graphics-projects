namespace RE330
{
    class NodeGroup : public Node
    {
    public:
        NodeGroup();

        virtual void draw(Matrix4 m, RenderContext rc, Camera c);

        // return copy of list to prevent modification
        list getChildren()
            {
                list<Node> copy = list<Node>(children);
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
        list<Node> children;
        list<Node>::iterator it;
    }
}
