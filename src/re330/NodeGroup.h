namespace RE330
{
    class NodeGroup : public Node
    {
    public:
        NodeGroup();

        void updateMatrix();
        virtual void draw(Matrix4 m, RenderContext rc, Camera c);
        List getChildren(); // return copy of list to prevent modification
        Node getChild(int index);
        void addChildNode(Node child);
        Node removeChild(int i);
        Node removeChild(Node child);
        int getNumChildren();

    private:
        List<Node> children;
    }
}
