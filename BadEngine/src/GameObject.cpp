#include "../lib/GameObject.h"

GameObject::GameObject(std::string Name, std::string Tag, int Layer)
    : name(Name), tag(Tag), layer(Layer), active(true), parent(nullptr), modelComponent(nullptr), isRotating(false) {
    localTransform = new Transform();
}

GameObject::~GameObject() {
    for (auto child : children)
    {
        delete child;
    }
    children.clear();
    delete localTransform;
}

void GameObject::setParent(GameObject* Parent)
{
    parent = Parent;
}

void GameObject::addChild(GameObject* Child)
{
    children.push_back(Child);
}

void GameObject::removeChild(GameObject* Child)
{
    auto it = std::find(children.begin(), children.end(), Child);

    if (it != children.end()) {
        children.erase(it);
    }
}

Transform* GameObject::getTransform()
{
    return localTransform;
}

Component* GameObject::getComponent()
{
    return new Component();
}

Component* GameObject::getComponentInChildren()
{
    return children.at(0)->getComponent();
}

Component* GameObject::getComponentInParent()
{
    return parent->getComponent();
}

void GameObject::addModelComponent(Model* model) {
    modelComponent = model;
}

Model* GameObject::getModelComponent() const {
    return modelComponent;
}

Transform* GameObject::getGlobalTransform() {
    if (parent == nullptr) return localTransform;
    Transform* globalTransform = new Transform(*localTransform);
    Transform* parentGlobalTransform = parent->getGlobalTransform();
    globalTransform->localPosition += parentGlobalTransform->localPosition;
    globalTransform->localRotation += parentGlobalTransform->localRotation;
    globalTransform->localScale += parentGlobalTransform->localScale;

    delete parentGlobalTransform;

    return globalTransform;
}

void GameObject::Move(glm::vec3 translation) {
    localTransform->localPosition += translation;
}

void GameObject::Update(glm::mat4 view, glm::mat4 perspective, float time) {
    if (modelComponent != nullptr) {
        glm::mat4 M = glm::translate(glm::mat4(1.f), localTransform->localPosition);
        if (isRotating) {
            M = glm::rotate(M, 100.f * glm::radians(time), glm::vec3(0.f, 0.f, 1.f));
        }
        M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
        modelComponent->setTransform(&M);
    }
}

void GameObject::setRotating(bool rotating) {
    isRotating = rotating;
}

YAML::Node GameObject::serialize() {
    YAML::Node node;
    node["name"] = this->name;
    node["tag"] = this->tag;
    node["layer"] = this->layer;
    node["transform"] = this->localTransform->serialize();
    node["model"] = this->modelComponent->serialize();
    return node;
}