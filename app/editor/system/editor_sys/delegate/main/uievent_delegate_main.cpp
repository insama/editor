#include "uievent_delegate_main.h"
#include "../../component/component.h"
#include "../../../tools/event_enum.h"
#include "../../../event_sys/event_sys.h"

bool UIEventDelegateMainObjList::OnCallEventMessage(UIObject * object, UIEventEnum e, const UIObject::UIEventDetails::Base & param)
{
    switch (e)
    {
    case UIEventEnum::kMOUSE:
        {
            auto & mouse = (const UIObject::UIEventDetails::Mouse &)param;
            if (mouse.mKey == 1 && mouse.mAct == 3)
            {
                object->GetState()->Pointer = Global::Ref().mEditorSys->mRootObject;

                std::vector<std::string> buffer;
                if (mouse.mObject == object)
                {
                    buffer.push_back("Add Object");
                }
                else
                {
                    auto & name = mouse.mObject->GetState()->Name;
                    buffer.push_back(        "Add Object");
                    buffer.push_back(        "Del Object");
                    buffer.push_back(SFormat("Rename Object/{0}~", name));

                    std::transform(
                        std::begin(Global::Ref().mCfgSys->At("res/cfg/editor/component.json", "Order")),
                        std::end(Global::Ref().mCfgSys->At("res/cfg/editor/component.json", "Order")),
                        std::back_inserter(buffer), [](const auto & pair) { return "Add Component/" + pair.mValue->ToString(); });
                }
                UIMenu::PopMenu(mouse.mObject, buffer);
            }

            if ((mouse.mKey == 0 || mouse.mKey == 1) && 
                (mouse.mAct == 2 || mouse.mAct == 3) && 
                mouse.mObject != object)
            {
                //  ѡ�нڵ�, ֪ͨ
                auto glObject = (GLObject *)mouse.mObject->GetState()->Pointer;
                Global::Ref().mEditorSys->mSelected.clear();
                Global::Ref().mEditorSys->mSelected.push_back(glObject);
                Global::Ref().mEventSys->Post(EventTypeEnum::kSELECT_GLOBJECT, nullptr);
            }
        }
        break;
    case UIEventEnum::kMENU:
        {
            auto menu = (const UIObject::UIEventDetails::Menu &)param;
            if (menu.mPath == "Add Object")
            {
                auto insert = (GLObject *)menu.mObject->GetState()->Pointer;
                auto name = Global::Ref().mEditorSys->GenerateObjectName(insert);

                //  ����GLObject
                auto glObject = new GLObject();
                insert->AddObject(glObject, name);

                //  ����UIObject
                auto raw  = mmc::JsonValue::Hash();
                raw->Insert(mmc::JsonValue::List(), "__Children");
                raw->Insert(mmc::JsonValue::Hash(), "__Property");
                raw->Insert(mmc::JsonValue::FromValue("2"), "__Property", "Type");
                raw->Insert(mmc::JsonValue::FromValue("0"), "__Property", "Align");
                raw->Insert(mmc::JsonValue::FromValue(name), "__Property", "Name");
                auto uiObject = UIParser::Parse(raw);
                uiObject->GetState()->Pointer = glObject;
                menu.mObject->AddObject(uiObject);
            }
            else if (menu.mPath == "Del Object")
            {
                auto glObject = (GLObject *)menu.mObject->GetState()->Pointer;
                menu.mObject->DelThis();
                glObject->DelThis();

                //  ɾ���ڵ�, ֪ͨ
                Global::Ref().mEditorSys->mSelected.clear();
                Global::Ref().mEventSys->Post(EventTypeEnum::kSELECT_GLOBJECT, nullptr);
            }
            else if (
                menu.mPath.at(0) == 'R' && menu.mPath.at(1) == 'e' && 
                menu.mPath.at(2) == 'n' && menu.mPath.at(3) == 'a' &&
                menu.mPath.at(4) == 'm' && menu.mPath.at(5) == 'e')
            {
                auto glObject = (GLObject *)menu.mObject->GetState()->Pointer;
                if (Global::Ref().mEditorSys->CheckRename(glObject, menu.mEdit))
                {
                    menu.mObject->GetState()->Name = menu.mEdit;
                }
            }
            else
            {
                auto name = menu.mPath.substr(std::strlen("Add Component/"));
                auto glObject = (GLObject *)menu.mObject->GetState()->Pointer;
                glObject->AddComponent(Component::Create(name));
                Global::Ref().mEventSys->Post(EventTypeEnum::kSELECT_GLOBJECT, nullptr);
            }
            std::cout
                << "Menu Key: " << menu.mPath << ' '
                << "Menu Edit: " << menu.mEdit << std::endl;
        }
        break;
    }
    return true;
}

bool UIEventDelegateMainResList::OnCallEventMessage(UIObject * object, UIEventEnum e, const UIObject::UIEventDetails::Base & param)
{
    return true;
}

bool UIEventDelegateMainComList::OnCallEventMessage(UIObject * object, UIEventEnum e, const UIObject::UIEventDetails::Base & param)
{
    if (UIEventEnum::kDELEGATE == e)
    {
        auto & arg = (const UIObject::UIEventDetails::Delegate &)param;
        if (arg.mType == 0)
        {
            _onwer = object;

            _listener.AddListen(EventTypeEnum::kSELECT_GLOBJECT, std::bind(&UIEventDelegateMainComList::OnRefreshComponent, this));
        }
    }
    return true;
}

void UIEventDelegateMainComList::OnRefreshComponent()
{ 
    _onwer->ClearObjects();

    //  ����
    if (!Global::Ref().mEditorSys->mSelected.empty())
    {
        auto glObject = Global::Ref().mEditorSys->mSelected.at(0);
        for (auto component : glObject->GetComponents())
        {
            auto header = new ComponentHeader(component->GetName());
            _onwer->AddObject(header);

            auto modifyfunc = std::bind(
                &Component::OnModifyProperty, component, 
                std::placeholders::_1, 
                std::placeholders::_2, 
                std::placeholders::_3);
            auto propertys = component->CollectProperty();
            for (const auto & property : propertys)
            {
                switch (property.mType)
                {
                case tools::ValueParser::kVEC2:
                    header->AddObject(new PropertyVector2(*(glm::vec2 *)property.mMember, property.mName, modifyfunc));
                    break;
                case tools::ValueParser::kFLOAT:
                    header->AddObject(new PropertyFloat(*(float *)property.mMember, property.mName, modifyfunc));
                    break;
                }
            }
        }
        
        std::cout << Global::Ref().mEditorSys->mSelected.at(0)->GetTag() << std::endl;
    }
}

bool UIEventDelegateMainStage::OnCallEventMessage(UIObject * object, UIEventEnum e, const UIObject::UIEventDetails::Base & param)
{
    return true;
}