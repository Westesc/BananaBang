class GraphNode
{
protected:
    GraphNode* parent;
    //Model* model;
    glm::mat4* worldTransform;
    glm::mat4* transform;
    glm::mat4* transformOnStart;
    std::vector<GraphNode*> children;
    bool isRotating = true;
    float dir;
    float x;
    float y;
    float z;
public:
    GraphNode(bool _isRotating /*Model* m = NULL*/)
    {
        isRotating = _isRotating;
        //this->model = m;
        parent = NULL;
        transform = new glm::mat4(1);
        worldTransform = new glm::mat4(1);
        transformOnStart = new glm::mat4(1);

    }
    ~GraphNode(void)
    {
        delete transform;
        delete worldTransform;
        for (unsigned int i = 0; i < children.size(); ++i) {
            delete children[i];
        }
    }
    void SetTransform(glm::mat4* matrix)
    {
        transform = matrix;
        *transformOnStart = *transform;
    }


    glm::mat4* GetTransform() { return transform; }

    glm::mat4* GetWorldTransform() { return worldTransform; }

    //Model* GetModel() { return model; }

    //void SetModel(Model* m) { model = m; }

    void AddChild(GraphNode* node)
    {
        children.push_back(node);
        node->parent = this;
    }

    void Update()
    {
        if (parent)
        {
            *worldTransform = *parent->worldTransform * (*transform);

            if (!isRotating)
            {
                *worldTransform = *parent->worldTransform * (*transformOnStart);
            }
        }
        else
        {
            *worldTransform = *transform;
        }
        /*if (model)
        {
            if (!parent)
            {
                model->setTransform(transformOnStart);
            }
            else {
                model->setTransform(worldTransform);
            }
        }*/
        for (GraphNode* node : children)
        {
            node->Update();
        }
    }
    void Draw()
    {
        //if (model) { model->Draw(); }

        for (GraphNode* node : children)
        {
            node->Draw();
        }
    }
    void Rotate(float angle, glm::vec3 axis) {
        *transform = glm::rotate(*transform, glm::radians(angle), axis);
    }
    void Translate(glm::vec3 translation) {
        *transform = glm::translate(*transform, translation);
    }
    void Scale(glm::vec3 scale) {
        *transform = glm::scale(*transform, scale);
    }
};
