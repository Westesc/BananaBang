#include "../lib/GameObject.h"
#include <glm/gtx/string_cast.hpp>

glm::vec3 nodeIntoVec3(YAML::Node node) {
    return glm::vec3(node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>());
}
YAML::Node vec3ToNode(glm::vec3 vector)
{
    YAML::Node node;
    node["x"] = vector.x;
    node["y"] = vector.y;
    node["z"] = vector.z;
    return node;
}

GameObject::GameObject(std::string Name, std::string Tag, int Layer)
    : name(Name), tag(Tag), layer(Layer), active(true), parent(nullptr), modelComponent(nullptr), isRotating(false) {
    localTransform = new Transform();
}
GameObject::GameObject(YAML::Node node) {
    this->name = node["name"].as<std::string>();
    this->tag = node["tag"].as<std::string>();
    this->layer = node["layer"].as<int>();
    this->isRotating = node["isRotating"].as<bool>();
    this->localTransform = new Transform(node["transform"]);
    this->modelComponent = new Model(node["model"]);
    if (isRotating) {
        this->setRotating(true, node["rotateSpeed"].as<float>(), nodeIntoVec3(node["rotateAxis"]));
    }
    
    if (node["children"]) {
        YAML::Node childrenNode = node["children"];
        for (auto childNode : childrenNode) {
            this->addChild(new GameObject(childNode));
        }
    }
}

GameObject::~GameObject() {
    delete localTransform;
    for (auto child : children)
    {
        delete child;
    }
    children.clear();
    
}

void GameObject::setParent(GameObject* Parent)
{
    parent = Parent;
}

void GameObject::addChild(GameObject* Child)
{
    Child->setParent(this);
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
    if (modelComponent != nullptr && uiComponent!= nullptr) {
        if (isRotating) {
            localTransform->localRotation.x += rotateAxis.x * rotateSpeed * glm::radians(time);
            localTransform->localRotation.y += rotateAxis.y * rotateSpeed * glm::radians(time);
            localTransform->localRotation.z += rotateAxis.z * rotateSpeed * glm::radians(time);
        }

        //modelComponent->updateBoundingBox(M);
        //std::cout << name << "M1:" << glm::to_string(M) << std::endl;
        //std::cout << "M2:" << glm::to_string(*modelComponent->getTransform()) << std::endl;
    }
    for (auto ch : children) {
        ch->Update(view, perspective,time);
    }
}

void GameObject::setRotating(bool rotating,float speed,glm::vec3 rotateAxis) {
    isRotating = rotating;
    rotateSpeed = speed;
    this->rotateAxis = rotateAxis;
}

void GameObject::checkResolveCollisions(GameObject* other, float deltaTime) {
    if (modelComponent->checkCollision(other->modelComponent)) {
        std::cout << "KOLIZJA" << std::endl;
        glm::vec3 displacement = modelComponent->calculateCollisionResponse(other->modelComponent);
        glm::vec3 otherDisplacement = -displacement;
        if (modelComponent->boundingBox != nullptr) {
            displacement *= 0.1f * deltaTime;
		}
        else {
			displacement *= deltaTime;
        }
        if (other->modelComponent->boundingBox != nullptr) {
			otherDisplacement *= 0.1f * deltaTime;
		}
        else {
			otherDisplacement *= deltaTime;
		}
        if (!(glm::any(glm::isnan(displacement)) || glm::any(glm::isinf(displacement)))) {
            localTransform->localPosition += displacement;
            other->localTransform->localPosition += otherDisplacement;
            modelComponent->setTransform(glm::translate(*modelComponent->getTransform(), displacement));
            other->modelComponent->setTransform(glm::translate(*other->modelComponent->getTransform(), otherDisplacement));
        }
    }
}

void GameObject::Draw(glm::mat4 view, glm::mat4 perspective) {
    if (modelComponent != nullptr) {
        glm::mat4 M = glm::translate(glm::mat4(1.f), localTransform->localPosition);
        M = glm::rotate(M, glm::radians(localTransform->localRotation.y), glm::vec3(0.f, 1.f, 0.f));
        M = glm::rotate(M, glm::radians(localTransform->localRotation.x), glm::vec3(1.0f, 0.f, 0.f));
        M = glm::rotate(M, glm::radians(localTransform->localRotation.z), glm::vec3(0.f, 0.f, 1.f));
        M = glm::scale(M, localTransform->localScale);
        modelComponent->setTransform(M);

        if (modelComponent->GetOutlineShader() != nullptr) {

            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            modelComponent->GetShader()->use();
            modelComponent->GetShader()->setMat4("M", *modelComponent->getTransform());
            modelComponent->GetShader()->setMat4("view", view);
            modelComponent->GetShader()->setMat4("projection", perspective);
            modelComponent->Draw();

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            //glDisable(GL_DEPTH_TEST);


            modelComponent->GetOutlineShader()->use();
            modelComponent->GetOutlineShader()->setMat4("M", *modelComponent->getTransform());
            modelComponent->GetOutlineShader()->setMat4("view", view);
            modelComponent->GetOutlineShader()->setMat4("projection", perspective);
            modelComponent->GetOutlineShader()->setFloat("outlineScale", 0.03f);
            modelComponent->Draw(modelComponent->GetOutlineShader());

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);

        }
        else if (modelComponent->GetFillingShader() != nullptr) {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            modelComponent->GetShader()->use();
            modelComponent->GetShader()->setMat4("M", *modelComponent->getTransform());
            modelComponent->GetShader()->setMat4("view", view);
            modelComponent->GetShader()->setMat4("projection", perspective);
            modelComponent->Draw();

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);


            modelComponent->GetFillingShader()->use();
            modelComponent->GetFillingShader()->setMat4("M", *modelComponent->getTransform());
            modelComponent->GetFillingShader()->setMat4("view", view);
            modelComponent->GetFillingShader()->setMat4("projection", perspective);
            modelComponent->GetFillingShader()->setFloat("outlineScale", 0.03f);
            modelComponent->Draw(modelComponent->GetFillingShader());

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        else {

            modelComponent->GetShader()->use();
            modelComponent->GetShader()->setMat4("M", *modelComponent->getTransform());
            modelComponent->GetShader()->setMat4("view", view);
            modelComponent->GetShader()->setMat4("projection", perspective);
            modelComponent->Draw();
        }
        /*if (modelComponent->boundingBox != nullptr) {
            modelComponent->DrawBoundingBoxes(modelComponent->GetShader(), *modelComponent->getTransform());
        }
        else if (modelComponent->capsuleCollider != nullptr) {
            modelComponent->UpdateCollider(*modelComponent->getTransform());
        }
        modelComponent->setPrevTransform(*modelComponent->getTransform());*/
    }
    else if (uiComponent != nullptr) {
        uiComponent->Draw(localTransform);
    }
    for (auto ch : children) {
        if (ch->isVisible) {
            ch->Draw(view, perspective);
        }
    }
}

void GameObject::Draw(Shader* shader) {
    if (modelComponent != nullptr) {
        glm::mat4 M = glm::translate(glm::mat4(1.f), localTransform->localPosition);
        M = glm::rotate(M, glm::radians(localTransform->localRotation.y), glm::vec3(0.f, 1.f, 0.f));
        M = glm::rotate(M, glm::radians(localTransform->localRotation.x), glm::vec3(1.0f, 0.f, 0.f));
        M = glm::rotate(M, glm::radians(localTransform->localRotation.z), glm::vec3(0.f, 0.f, 1.f));
        M = glm::scale(M, localTransform->localScale);
        modelComponent->setTransform(M);
        modelComponent->Draw(shader);
    }
    for (auto ch : children) {
        if (ch->isVisible) {
            ch->Draw(shader);
        }
    }
}

YAML::Node GameObject::serialize() {
    YAML::Node node;
    node["name"] = this->name;
    node["tag"] = this->tag;
    node["layer"] = this->layer;
    node["isRotating"] = this->isRotating;
    if (isRotating) {
        node["rotateSpeed"] = this->rotateSpeed;
        node["rotateAxis"] = vec3ToNode(this->rotateAxis);
    }
    node["transform"] = this->localTransform->serialize();
    node["model"] = this->modelComponent->serialize();
    for (auto child : children) {
        node["children"].push_back(child->serialize());
    }
    return node;
}

void GameObject::setVisible(bool visible) {
	isVisible = visible;
}

void GameObject::lightSetting(glm::vec3 viewPos, glm::vec3 lightPos, glm::vec3 lightColor) {
    if (modelComponent != nullptr) {
        getModelComponent()->GetShader()->use();
        getModelComponent()->GetShader()->setVec3("viewPos", viewPos);
        getModelComponent()->GetShader()->setVec3("lightPos", lightPos);
        getModelComponent()->GetShader()->setVec3("lightColor", lightColor);
    }
    for (auto ch : children) {
        ch->lightSetting(viewPos, lightPos, lightColor);
    }
}
void GameObject::shadowSetting(glm::mat4 LSMatrix) {
    if (modelComponent != nullptr) {
        getModelComponent()->GetShader()->use();
        getModelComponent()->GetShader()->setMat4("LSMatrix", LSMatrix);
    }
    for (auto ch : children) {
        ch->shadowSetting(LSMatrix);
    }
}

void GameObject::timeSetting(float time, glm::vec2 iResolution) {
    if (modelComponent != nullptr) {
        modelComponent->GetShader()->use();
        modelComponent->GetShader()->setFloat("iTime", time);
        modelComponent->GetShader()->setVec2("iResolution", iResolution);
    }
}

void GameObject::addColider() {
    modelComponent->addCollider(1, localTransform->localPosition);
    boundingBox = new BoundingBox(glm::vec3(0.f), glm::vec3(0.f));
    *boundingBox = *modelComponent->boundingBox;
}