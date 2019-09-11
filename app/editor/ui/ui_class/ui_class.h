#pragma once

#include "../base.h"

class UIClass {
public:
    struct EventDetails {
        //  返回按下的状态键
        static int CheckStateKey();

        struct Base {
            mutable UIClass * mObject;

            Base(UIClass * object = nullptr) : mObject(object)
            {  }
        };

        //  键盘事件
        struct Key : Base {
            int mKey;
            int mAct;   //  0, 1, 2, 3 => 按下, 抬起, 单击, 单击延迟
            int mState; //  1, 2, 4    => alt, ctrl, shift

            Key() { memset(this, sizeof(Key), 0); }
        };

        //  鼠标事件
        struct Mouse : Base {
            int mKey;           //  0, 1, 2       => 左键, 右键, 中键
            int mAct;           //  0, 1, 2, 3, 4 => 悬浮, 按下, 抬起, 单击, 双击
            int mState;         //  1, 2, 4       => alt, ctrl, shift
            glm::vec2 mMouse;   //  鼠标坐标

            Mouse(const int act, const int key, UIClass * object = nullptr)
                : Base(object)
                , mAct(act)
                , mKey(key)
            {
                mMouse.x = ImGui::GetMousePos().x;
                mMouse.y = ImGui::GetMousePos().y;
                mState = CheckStateKey();
            }
        };

        //  编辑文本事件
        struct EditText : Base {
            std::string mString;

            EditText(const std::string & string, UIClass * object = nullptr)
                : Base(object)
                , mString(string)
            { }
        };
    };

public:
    template <class T = UIState>
    T * GetState() { return (T *)_state; }
    
    std::vector<UIClass *> GetChildren(UITypeEnum type) const;
    std::vector<UIClass *> & GetChildren();
    void AddChild(UIClass * child);
    void DelChild(UIClass * child);
    void ClearChild();
    UIClass * GetRoot();
    UIClass * GetParent();

    void Render(float dt);
    void ResetLayout();
    void ApplyLayout();

    glm::vec4 CalcStretech(DirectEnum direct, const glm::vec2 & offset) const;
    glm::vec2 ToWorldCoord(const glm::vec2 & coord = glm::vec2(0)) const;
    glm::vec4 ToLocalCoord(const glm::vec4 & coord) const;
    glm::vec2 ToLocalCoord(const glm::vec2 & coord) const;
    glm::vec4 ToWorldRect() const;

    //  const 重载版本
    UITypeEnum GetType() const
    {
        return _type;
    }

    const UIClass * GetRoot() const
    {
        return const_cast<UIClass *>(this)->GetRoot();
    }

    const UIClass * GetParent() const
    {
        return const_cast<UIClass *>(this)->GetParent();
    }

    const std::vector<UIClass *> & GetChildren() const
    {
        return const_cast<UIClass *>(this)->GetChildren();
    }

    template <class T = UIState>
    const T * GetState() const
    {
        return const_cast<UIClass *>(this)->GetState<T>();
    }

protected:
    UIClass(UITypeEnum type, UIState * state) 
        : _type(type)
        , _state(state)
        , _parent(nullptr)
    { }

    void LockPosition();
    virtual bool OnEnter();
    virtual void OnLeave(bool ret);
    virtual void OnResetLayout();
    virtual void OnApplyLayout();
    virtual void OnRender(float dt) = 0;
    virtual bool OnCallEventMessage(UIEventEnum e, const std::any & param) { return false; }

    //  事件处理
    void DispatchEventM();
    void DispatchEventK();
    bool CallEventMessage(UIEventEnum e, const EventDetails::Base & param);
    bool PostEventMessage(UIEventEnum e, const EventDetails::Base & param);

private:
    bool DispatchEventM(UIEventEnum e, const EventDetails::Mouse & param);

private:
    UITypeEnum             _type;
    UIState *              _state;
    UIClass *              _parent;
    std::vector<UIClass *> _children;
};

class UIClassLayout : public UIClass {
public:
    UIClassLayout(UIState * state): UIClass(UITypeEnum::kLAYOUT, state)
    { }

private:
    virtual bool OnEnter() override;
    virtual void OnLeave(bool ret) override;
    virtual void OnResetLayout() override;
    virtual void OnApplyLayout() override;
    virtual void OnRender(float dt) override;

    bool IsCanStretch(DirectEnum edge);
    bool IsCanStretch(DirectEnum edge, const glm::vec2 & offset);
    void HandleStretch();
};

class UIClassTreeBox : public UIClass {
public:
    UIClassTreeBox(UIState * state): UIClass(UITypeEnum::kTREEBOX, state)
    { }

private:
    virtual bool OnEnter() override;
    virtual void OnLeave(bool ret) override;
    virtual void OnRender(float dt) override;
};

class UIClassTextBox : public UIClass {
public:
    UIClassTextBox(UIState * state) : UIClass(UITypeEnum::kTEXTBOX, state)
    { }

private:
    virtual void OnRender(float dt) override;
};

class UIClassImageBox : public UIClass {
public:
    UIClassImageBox(UIState * state): UIClass(UITypeEnum::kIMAGEBOX, state)
    { }

private:
    virtual void OnRender(float dt) override;
};

class UIClassComboBox : public UIClass {
public:
    UIClassComboBox(UIState * state) : UIClass(UITypeEnum::kCOMBOBOX, state)
    { }

private:
    virtual bool OnEnter() override;
    virtual void OnLeave(bool ret) override;
    virtual void OnRender(float dt) override;
    virtual bool OnCallEventMessage(UIEventEnum e, const std::any & param) override;
};

class UIClassUICanvas : public UIClass {
public:
    UIClassUICanvas(UIState * state) : UIClass(UITypeEnum::kUICONVAS, state)
    { }

private:
    virtual void OnRender(float dt) override;
};

class UIClassGLCanvas : public UIClass {
public:
    UIClassGLCanvas(UIState * state) : UIClass(UITypeEnum::kGLCONVAS, state)
    { }

private:
    virtual void OnRender(float dt) override;
};