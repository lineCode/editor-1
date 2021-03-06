﻿#include "ui_delegate_main.h"

bool UIDelegateMainObjList::OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param)
{
    if (UIEventEnum::kInit == e)
    {
        auto & arg = (const UIEvent::Init &)param;
        mListener.Add(EventSys::TypeEnum::kOpenProject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kFreeProject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kInsertObject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kDeleteObject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kRenameObject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kSelectObject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kStateObject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kMoveObject, std::bind(&UIDelegateMainObjList::OnEvent, this, std::placeholders::_1, std::placeholders::_2), Global::Ref().mEventSys);
        return true;
    }

    if (!Global::Ref().mEditorSys->IsOpenProject()) { return false; }

    switch (e)
    {
    case UIEventEnum::kMouse:
        {
            auto & mouse = (const UIEvent::Mouse &)param;
            //  右键单击
            if (mouse.mKey == 1 && mouse.mAct == 3)
            {
                std::vector<std::string> buffer;
                if (mouse.mObject == GetOwner())
                {
                    buffer.push_back("Add Object");
                    UIMenu::PopMenu(mouse.mObject, buffer);
                }
                else
                {
                    auto & name = mouse.mObject->GetParent()->GetState()->Name;
                    buffer.push_back(        "Add Object");
                    buffer.push_back(        "Del Object");
                    buffer.push_back(SFormat("Rename Object\\{0}~", name));

                    std::transform(
                        std::begin(Global::Ref().mCfgSys->At("res/cfg/editor/component.json", "Order")),
                        std::end(  Global::Ref().mCfgSys->At("res/cfg/editor/component.json", "Order")),
                        std::back_inserter(buffer), [](const auto & pair) { return "Add Component\\" + pair.mVal->ToString(); });
                    UIMenu::PopMenu(mouse.mObject->GetParent(), buffer);
                }
            }

            if (mouse.mKey == 2 && mouse.mAct == 3 && mouse.mObject == GetOwner())
            {
                std::string                   arg0("image");
                std::function<void(Res::Ref)> arg1 = nullptr;
                Global::Ref().mUISys->OpenWindow(UIFile_Explorer, std::make_tuple(arg0, arg1));
            }

            //  左键单击
            if (mouse.mKey == 0 && mouse.mAct == 3 && mouse.mObject != GetOwner())
            {
                auto objectID = _obj2id.at(mouse.mObject->GetParent());
                if      (mouse.mObject->GetParent()->GetObjects().at(0) == mouse.mObject)
                {
                    //  激活
                    if (auto glo = Global::Ref().mEditorSys->GetProject()->GetObject(objectID); glo->HasState(GLObject::StateEnum::kActive))
                    {
                        Global::Ref().mEditorSys->OptStateSubObject(objectID, GLObject::StateEnum::kActive);
                    }
                    else
                    {
                        Global::Ref().mEditorSys->OptStateAddObject(objectID, GLObject::StateEnum::kActive);
                    }
                }
                else if (mouse.mObject->GetParent()->GetObjects().at(1) == mouse.mObject)
                {
                    //  锁定
                    if (auto glo = Global::Ref().mEditorSys->GetProject()->GetObject(objectID); glo->HasState(GLObject::StateEnum::kLocked))
                    {
                        Global::Ref().mEditorSys->OptStateSubObject(objectID, GLObject::StateEnum::kLocked);
                    }
                    else
                    {
                        Global::Ref().mEditorSys->OptStateAddObject(objectID, GLObject::StateEnum::kLocked);
                    }
                }
            }

            if (    mouse.mObject != GetOwner()
                &&  (mouse.mKey == 0 || mouse.mKey == 1)
                &&  (mouse.mAct == 2 || mouse.mAct == 3) 
                &&  mouse.mObject->GetParent()->GetObjects().at(2) == mouse.mObject)
            {
                Global::Ref().mEditorSys->OptSelectObject(_obj2id.at(mouse.mObject->GetParent()), true);
            }
        }
        return true;
    case UIEventEnum::kMenu:
        {
            auto & menu = (const UIEvent::Menu &)param;
            if (menu.mPath == "Add Object")
            {
                auto objectID = _obj2id.at(menu.mObject);
                auto parent = Global::Ref().mEditorSys->GetProject()->GetObject(objectID);
                auto object = Global::Ref().mEditorSys->GetProject()->NewObject();
                auto name = Global::Ref().mEditorSys->ObjectName(parent);
                (void)object->SetName(name);
                Global::Ref().mEditorSys->OptInsertObject(object,parent);
            }
            else if (menu.mPath == "Del Object")
            {
                Global::Ref().mEditorSys->OptDeleteObject(_obj2id.at(menu.mObject));
            }
            else if (
                menu.mPath.at(0) == 'R' && menu.mPath.at(1) == 'e' && 
                menu.mPath.at(2) == 'n' && menu.mPath.at(3) == 'a' &&
                menu.mPath.at(4) == 'm' && menu.mPath.at(5) == 'e')
            {
                Global::Ref().mEditorSys->OptRenameObject(_obj2id.at(menu.mObject), menu.mEdit);
            }
            else if (menu.mPath.at(0) == 'A' && menu.mPath.at(1) == 'd' &&
                     menu.mPath.at(2) == 'd' && menu.mPath.at(3) == ' ' &&
                     menu.mPath.at(4) == 'C' && menu.mPath.at(5) == 'o' &&
                     menu.mPath.at(6) == 'm' && menu.mPath.at(7) == 'p')
            {
                Global::Ref().mEditorSys->OptAppendComponent(_obj2id.at(menu.mObject), Component::Create(tools::GetFileFullName(menu.mPath)));
            }
        }
        return true;
    case UIEventEnum::kDrag:
        {
            auto & drag = (const UIEvent::Drag &)param;
            if (drag.mAct == 1 && _obj2id.find(drag.mDragObj->GetParent()) != _obj2id.end())
            {
                return true;
            }
            if (drag.mAct == 2 && _obj2id.find(drag.mDragObj->GetParent()) != _obj2id.end())
            {
                auto pos = 0;
                auto dragObj = drag.mDragObj->GetParent();
                auto freeObj = drag.mFreeObj->GetParent();
                if (drag.mFreeObj == GetOwner())
                {
                    freeObj = _id2obj.at(Global::Ref().mEditorSys->GetProject()->GetObject()->GetID());
                }
                else
                {
                    if (drag.mDirect == DirectEnum::kU || drag.mDirect == DirectEnum::kL) { pos = 1; }
                    if (drag.mDirect == DirectEnum::kD || drag.mDirect == DirectEnum::kR) { pos = 2; }
                }
                Global::Ref().mEditorSys->OptMoveObject(_obj2id.at(dragObj), _obj2id.at(freeObj), pos);
                return true;
            }
        }
        return false;
    }
    return false;
}

void UIDelegateMainObjList::InitObjects(const SharePtr<UIObject> & uiparent, const SharePtr<GLObject> & glparent)
{
    for (auto & object : glparent->GetObjects())
    {
        auto obj = NewObject(object->GetID(), object->GetName());
        InitObjects(obj,   object);
        if (uiparent == GetOwner())
        {
            uiparent->InsertObject(obj);
        }
        else
        {
            uiparent->GetObject({ uiparent->GetState()->Name })->InsertObject(obj);
        }
    }
}

SharePtr<UIObject> UIDelegateMainObjList::NewObject(uint id, const std::string & name)
{
    auto layout = mmc::Json::Hash();
    (void)layout->Insert(mmc::Json::List(), "__Children");
    (void)layout->Insert(mmc::Json::Hash(), "__Property");
    (void)layout->Insert(mmc::Json::FromValue("0"), "__Property", "Type");
    (void)layout->Insert(mmc::Json::FromValue(name), "__Property", "Name");
    (void)layout->Insert(mmc::Json::FromValue("true"), "__Property", "IsSameline");
    (void)layout->Insert(mmc::Json::FromValue("true"), "__Property", "IsCanDragMove");
    (void)layout->Insert(mmc::Json::FromValue("true"), "__Property", "IsCanDragFree");
    (void)layout->Insert(mmc::Json::FromValue("false"), "__Property", "IsShowBorder");

    //  Active
    auto active = mmc::Json::Hash();
    (void)active->Insert(mmc::Json::List(), "__Children");
    (void)active->Insert(mmc::Json::Hash(), "__Property");
    (void)active->Insert(mmc::Json::FromValue("3"), "__Property", "Type");
    (void)active->Insert(mmc::Json::FromValue("| X |"), "__Property", "Name");
    (void)active->Insert(mmc::Json::FromValue("false"), "__Property", "IsCanDragMove");
    (void)active->Insert(mmc::Json::FromValue("false"), "__Property", "IsCanDragFree");
    (void)layout->Insert(active, "__Children", 0);

    //  Locked
    auto locked = mmc::Json::Hash();
    (void)locked->Insert(mmc::Json::List(), "__Children");
    (void)locked->Insert(mmc::Json::Hash(), "__Property");
    (void)locked->Insert(mmc::Json::FromValue("3"), "__Property", "Type");
    (void)locked->Insert(mmc::Json::FromValue("| O |"), "__Property", "Name");
    (void)locked->Insert(mmc::Json::FromValue("false"), "__Property", "IsCanDragMove");
    (void)locked->Insert(mmc::Json::FromValue("false"), "__Property", "IsCanDragFree");
    (void)layout->Insert(locked, "__Children", 1);

    //  Tree
    auto tree = mmc::Json::Hash();
    (void)tree->Insert(mmc::Json::List(), "__Children");
    (void)tree->Insert(mmc::Json::Hash(), "__Property");
    (void)tree->Insert(mmc::Json::FromValue("2"), "__Property", "Type");
    (void)tree->Insert(mmc::Json::FromValue(name), "__Property", "Name");
    (void)tree->Insert(mmc::Json::FromValue("true"), "__Property", "IsCanDragMove");
    (void)tree->Insert(mmc::Json::FromValue("true"), "__Property", "IsCanDragFree");
    (void)layout->Insert(tree, "__Children", 2);

    auto object = UIParser::Parse(layout);
    _id2obj.insert(std::make_pair(id, object));
    _obj2id.insert(std::make_pair(object, id));
    return object;
}

void UIDelegateMainObjList::OnEvent(EventSys::TypeEnum type, const std::any & param)
{
    switch (type)
    {
    case EventSys::TypeEnum::kOpenProject:
        OnEventOpenProject();
        break;
    case EventSys::TypeEnum::kFreeProject:
        OnEventFreeProject();
        break;
    case EventSys::TypeEnum::kInsertObject:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>> &>(param);
            OnEventInsertObject(std::get<0>(value));
        }
        break;
    case EventSys::TypeEnum::kDeleteObject:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>> &>(param);
            OnEventDeleteObject(std::get<0>(value));
        }
        break;
    case EventSys::TypeEnum::kRenameObject:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>,std::string> &>(param);
            OnEventRenameObject(std::get<0>(value), std::get<1>(value));
        }
        break;
    case EventSys::TypeEnum::kStateObject:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>, uint, uint> &>(param);
            OnEventStateObject(std::get<0>(value), std::get<1>(value), std::get<2>(value));
        }
        break;
    case EventSys::TypeEnum::kSelectObject:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>, bool, bool> &>(param);
            OnEventSelectObject(std::get<0>(value), std::get<1>(value), std::get<2>(value));
        }
        break;
    case EventSys::TypeEnum::kMoveObject:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>, SharePtr<GLObject>, uint> &>(param);
            OnEventMoveObject(std::get<0>(value), std::get<1>(value), std::get<2>(value));
        }
        break;
    }
}

void UIDelegateMainObjList::OnEventOpenProject()
{
    ASSERT_LOG(_obj2id.empty(), "");
    ASSERT_LOG(_id2obj.empty(), "");
    auto root = Global::Ref().mEditorSys->GetProject()->GetObject();
    _id2obj.insert(std::make_pair(root->GetID(), GetOwner()));
    _obj2id.insert(std::make_pair(GetOwner(), root->GetID()));
    InitObjects(GetOwner(), Global::Ref().mEditorSys->GetProject()->GetObject());
}

void UIDelegateMainObjList::OnEventFreeProject()
{
    _obj2id.clear();
    _id2obj.clear();
    GetOwner()->ClearObjects();
}

void UIDelegateMainObjList::OnEventInsertObject(const SharePtr<GLObject> & object)
{
    if (object->GetParent() == Global::Ref().mEditorSys->GetProject()->GetObject())
    {
        auto uiobject = NewObject(object->GetID(), object->GetName());
        _id2obj.at(object->GetParent()->GetID())->InsertObject(uiobject);
    }
    else
    {
        auto parent = _id2obj.at(object->GetParent()->GetID());
        auto uiobject = NewObject(object->GetID(), object->GetName());
        parent->GetObject({ parent->GetState()->Name })->InsertObject(uiobject);
    }

    auto stage = CastPtr<UIObjectGLCanvas>(GetOwner()->GetParent()->GetObject({"EditorObjStage", "Canvas"}));
    auto coord = glm::vec2(stage->GetState()->Move.z * 0.5f,
                           stage->GetState()->Move.w * 0.5f);
    coord = object->GetParent()->WorldToLocal(stage->ProjectWorld(coord));
    object->GetTransform()->Position(coord.x, coord.y);
}

void UIDelegateMainObjList::OnEventDeleteObject(const SharePtr<GLObject>& object)
{
    auto uiobject = _id2obj.at(object->GetID());
    _id2obj.erase(object->GetID());
    _obj2id.erase(uiobject);
    uiobject->DeleteThis();
}

void UIDelegateMainObjList::OnEventRenameObject(const SharePtr<GLObject>& object, const std::string & name)
{
    auto uiobject0 = _id2obj.at(object->GetID());
    auto uiobject1 = uiobject0->GetObject({name});
    uiobject0->GetState()->Name = object->GetName();
    uiobject1->GetState()->Name = object->GetName();
}

void UIDelegateMainObjList::OnEventSelectObject(const SharePtr<GLObject> & object, bool select, bool multi)
{
    auto uiobject = _id2obj.at(object->GetID());
    uiobject->GetObject(
        { uiobject->GetState()->Name }
    )->GetState()->IsSelect = select;
}

void UIDelegateMainObjList::OnEventStateObject(const SharePtr<GLObject> & object, uint state0, uint state1)
{
    auto active = object->HasState(GLObject::StateEnum::kActive);
    auto locked = object->HasState(GLObject::StateEnum::kLocked);
    _id2obj.at(object->GetID())->GetObjects().at(0)->GetState()->Color = active ? glm::vec4(1) : glm::vec4(1, 0, 0, 1);
    _id2obj.at(object->GetID())->GetObjects().at(1)->GetState()->Color = locked ? glm::vec4(1, 0, 0, 1) : glm::vec4(1);
}

void UIDelegateMainObjList::OnEventMoveObject(const SharePtr<GLObject> & object, const SharePtr<GLObject> & parent, uint pos)
{
    auto tree = GetOwner();
    auto uiobject = _id2obj.at(object->GetID());
    auto uiparent = _id2obj.at(parent->GetID());
    if (parent != Global::Ref().mEditorSys->GetProject()->GetObject())
    { 
        tree = uiparent->GetObject({ uiparent->GetState()->Name });
    }
    uiobject->DeleteThis(); tree->InsertObject(uiobject);

    auto & objects = tree->GetObjects();
    auto iter = std::next(objects.begin(), pos);
    std::move_backward(iter, std::prev(objects.end()), objects.end());
    *iter = uiobject;
}

bool UIDelegateMainResList::OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param)
{
    return true;
}

bool UIDelegateMainComList::OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param)
{
    if (UIEventEnum::kInit == e)
    {
        auto & arg = (const UIEvent::Init &)param;
        mListener.Add(EventSys::TypeEnum::kSelectObject, std::bind(
            &UIDelegateMainComList::OnEvent, this, 
            std::placeholders::_1, 
            std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kAppendComponent, std::bind(
            &UIDelegateMainComList::OnEvent, this,
            std::placeholders::_1,
            std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kDeleteComponent, std::bind(
            &UIDelegateMainComList::OnEvent, this,
            std::placeholders::_1,
            std::placeholders::_2), Global::Ref().mEventSys);
    }
    return false;
}

void UIDelegateMainComList::OnEvent(EventSys::TypeEnum type, const std::any & param)
{
    switch (type)
    {
    case EventSys::TypeEnum::kSelectObject:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>, bool, bool> &>(param);
            OnEventSelectObject(std::get<0>(value), std::get<1>(value), std::get<2>(value));
        }
        break;
    case EventSys::TypeEnum::kAppendComponent:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>, SharePtr<Component>> &>(param);
            OnEventAppendComponent(std::get<0>(value), std::get<1>(value));
        }
        break;
    case EventSys::TypeEnum::kDeleteComponent:
        {
            auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>, SharePtr<Component>> &>(param);
            OnEventDeleteComponent(std::get<0>(value), std::get<1>(value));
        }
        break;
    }
}

void UIDelegateMainComList::OnEventSelectObject(const SharePtr<GLObject> & object, bool select, bool multi)
{
    GetOwner()->ClearObjects();

    if (select)
    {
        for (auto component : object->GetComponents())
        {
            auto header = std::create_ptr<UIComponentHeader>(component);
            GetOwner()->InsertObject(header);

            for (auto property : component->CreateUIPropertys())
            {
                header->InsertObject(property);
            }
        }
    }
}

void UIDelegateMainComList::OnEventAppendComponent(const SharePtr<GLObject> & object, const SharePtr<Component> & component)
{
    auto header = std::create_ptr<UIComponentHeader>(component);
    GetOwner()->InsertObject(header);

    for (auto property : component->CreateUIPropertys())
    {
        header->InsertObject(property);
    }
}

void UIDelegateMainComList::OnEventDeleteComponent(const SharePtr<GLObject> & object, const SharePtr<Component> & component)
{
    auto & components = component->GetOwner()->GetComponents();
    auto it = std::find(components.begin(), components.end(), component);
    GetOwner()->DeleteObject(std::distance(components.begin(), it));
}

bool UIDelegateMainStage::OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param)
{
    if (UIEventEnum::kInit == e)
    {
        auto & arg = (const UIEvent::Init &)param;
        mListener.Add(EventSys::TypeEnum::kOpenProject, std::bind(
            &UIDelegateMainStage::OnEvent, this,
            std::placeholders::_1,
            std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kFreeProject, std::bind(
            &UIDelegateMainStage::OnEvent, this,
            std::placeholders::_1,
            std::placeholders::_2), Global::Ref().mEventSys);
        mListener.Add(EventSys::TypeEnum::kSelectObject, std::bind(
            &UIDelegateMainStage::OnEvent, this,
            std::placeholders::_1,
            std::placeholders::_2), Global::Ref().mEventSys);
        return true;
    }
    return false;
}

void UIDelegateMainStage::OnEvent(EventSys::TypeEnum type, const std::any & param)
{
    switch (type)
    {
    case EventSys::TypeEnum::kOpenProject:
        OnEventOpenProject();
        break;
    case EventSys::TypeEnum::kFreeProject:
        OnEventFreeProject();
        break;
    case EventSys::TypeEnum::kSelectObject:
        auto & value = std::any_cast<const std::tuple<SharePtr<GLObject>, bool, bool> &>(param);
        OnEventSelectObject(std::get<0>(value), std::get<1>(value), std::get<2>(value));
        break;
    }
}

void UIDelegateMainStage::OnEventOpenProject()
{
}

void UIDelegateMainStage::OnEventFreeProject()
{
    CastPtr<UIObjectGLCanvas>(GetOwner())->OpSelectedClear();
}

void UIDelegateMainStage::OnEventSelectObject(const SharePtr<GLObject> & object, bool select, bool multi)
{
    CastPtr<UIObjectGLCanvas>(GetOwner())->OpEditObject(nullptr);
    CastPtr<UIObjectGLCanvas>(GetOwner())->OpSelected(object, select);
}

bool UIDelegateMainGlobal::OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param)
{
    if (e == UIEventEnum::kMenu)
    {
        auto & menu = (const UIEvent::Menu &)param;
        if (menu.mPath == "Menu\\New Project")
        {
            Global::Ref().mEditorSys->OptNewProject("1.proj");
        }
        else if (menu.mPath == "Menu\\Open Project")
        {
            Global::Ref().mEditorSys->OptOpenProject("1.proj");
        }
        else if (menu.mPath == "Menu\\Save Project")
        {
            Global::Ref().mEditorSys->OptSaveProject("");
        }
        else if (menu.mPath == "Menu\\Free Project")
        {
            Global::Ref().mEditorSys->OptFreeProject();
        }
        else if (menu.mPath == "Style\\Drak")
        {
            ImGui::StyleColorsDark();
        }
        else if (menu.mPath == "Style\\Light")
        {
            ImGui::StyleColorsLight();
        }
        else if (menu.mPath == "Style\\Classic")
        {
            ImGui::StyleColorsClassic();
        }
        return true;
    }
    return false;
}
