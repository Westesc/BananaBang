#include "../lib/GameObject.h"

GameObject::GameObject(std::string Name, std::string Tag, int Layer)
    : name(Name), tag(Tag), layer(Layer), active(true), parent(nullptr), modelComponent(nullptr) {
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