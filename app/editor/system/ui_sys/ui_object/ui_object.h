#pragma once

#include "../include.h"
#include "../ui_event.h"
#include "../ui_state/ui_state.h"

class UIObject: public std::enable_shared_from_this<UIObject> {
public:
    UIObject(UITypeEnum type = UITypeEnum::kOther, UIState * state = new UIState())
        : mType(type)
        , mState(state)
        , mParent(nullptr)
        , mVisible(true)
    { }

    virtual ~UIObject()
    { }

    template <class T = UIState>
    T * GetState() { return (T *)mState.get(); }
    
    SharePtr<UIObject> GetObject(const std::initializer_list<std::string> & list);
    std::vector<SharePtr<UIObject>>   GetObjects(UITypeEnum type);
    std::vector<SharePtr<UIObject>> & GetObjects();
    void InsertObject(const SharePtr<UIObject> & object);
    void DeleteObject(const SharePtr<UIObject> & object);
    void DeleteObject(size_t index);
    void ClearObjects();
    void DeleteThis();
    SharePtr<UIObject> GetParent();
    SharePtr<UIObject> GetObject();
    UITypeEnum GetType();
    bool IsVisible();

    void ResetLayout();
    void ApplyLayout();
    void Render(float dt, bool visible = true);

    glm::vec4 CalcStretech(DirectEnum direct, const glm::vec2 & offset);
    glm::vec2 ToWorldCoord(const glm::vec2 & coord = glm::vec2(0));
    glm::vec4 ToLocalCoord(const glm::vec4 & coord);
    glm::vec2 ToLocalCoord(const glm::vec2 & coord);
    glm::vec2 ToLocalCoordFromImGUI();
    glm::vec4 ToWorldRect();

    //  绑定事件委托, 事件将被传递到委托中
    void BindDelegate(UIEvent::DelegateHandler * delegate);
    void WakeInit(const std::any & param);

protected:
    void RenderDrag();
    void UpdateSize();
    void UpdateMove();
    virtual bool OnEnter();
    virtual void OnLeave(bool ret);
    virtual void OnResetLayout();
    virtual void OnApplyLayout();

    //  事件处理
    void DispatchEventKey();
    SharePtr<UIObject> DispatchEventKey(const UIEvent::Key & param);
    void DispatchEventDrag();
    SharePtr<UIObject> DispatchEventDrag(const UIEvent::Drag & param);
    void DispatchEventMouse();
    SharePtr<UIObject> DispatchEventMouse(const UIEvent::Mouse & param);
    virtual bool OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param);

private:
    SharePtr<UIObject> CallEventMessage(UIEventEnum e, const UIEvent::Event & param);
public:
    SharePtr<UIObject> PostEventMessage(UIEventEnum e, const UIEvent::Event & param);

private:
    bool                                mVisible;       //  标记当前节点是否渲染
    UITypeEnum                          mType;
    UIObject *                          mParent;
    UniquePtr<UIState>                  mState;
    std::vector<SharePtr<UIObject>>     mChildren;
    UniquePtr<UIEvent::DelegateHandler> _delegate;
};

class UIObjectLayout : public UIObject {
public:
    UIObjectLayout();

    void Close();

private:
    virtual bool OnEnter() override;
    virtual void OnLeave(bool ret) override;
    virtual void OnResetLayout() override;
    virtual void OnApplyLayout() override;

    bool IsCanStretch(DirectEnum edge);
    bool IsCanStretch(DirectEnum edge, const glm::vec2 & offset);
    void HandleStretch();
};

class UIObjectTreeBox : public UIObject {
public:
    UIObjectTreeBox();

private:
    virtual bool OnEnter() override;
    virtual void OnLeave(bool ret) override;
};

class UIObjectTextBox : public UIObject {
public:
    UIObjectTextBox();

    void SetText(const std::string & text)
    {
        GetState<UIStateTextBox>()->mBuffer = text;
    }
private:
    virtual bool OnEnter() override;
};

class UIObjectImageBox : public UIObject {
public:
    UIObjectImageBox();

private:
    virtual bool OnEnter() override;
};

class UIObjectComboBox : public UIObject {
public:
    UIObjectComboBox();

private:
    virtual bool OnEnter() override;
    virtual void OnLeave(bool ret) override;
    virtual bool OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param) override;
};

class UIObjectUICanvas : public UIObject {
public:
    UIObjectUICanvas();
};

class UIObjectGLCanvas : public UIObject {
private:
    static const auto VAL_TrackPointLine    = 2;            //  追踪点线宽
    static const auto VAL_TrackPointSize    = 10;           //  追踪点大小
    static const glm::vec4 VAL_TrackPointColors[32];        //  追踪点颜色

public:
    UIObjectGLCanvas();
    RenderPipline::MatrixStack & GetMatrixStack();
    void Post(const RenderPipline::PostCommand  & cmd);
    void Post(const RenderPipline::TargetCommand & cmd);
    void Post(const RenderPipline::FowardCommand & cmd);
    void Post(const SharePtr<RawProgram> & program, const glm::mat4 & transform);

    //  交互操作
    void OpEditObject(const SharePtr<GLObject> & object);
    void OpDragSelects(const glm::vec2 & worldBeg, 
                       const glm::vec2 & worldEnd);
    void OpSelected(const SharePtr<GLObject> & object, bool selected);
    void OpSelectedClear();

    glm::vec2 ProjectScreen(const glm::vec2 & world);
    glm::vec2 ProjectWorld(const glm::vec2 & screen);

    void CanvasToFile(const std::string & name);

private:
    void HandleFowardCommands(RenderPipline::TargetCommand & command);
    void HandlePostCommands(  RenderPipline::TargetCommand & command);
    void CollCommands();
    void CallCommands();

    void DrawTrackPoint();
    void DrawSelectRect();

    virtual bool OnEnter() override;
    virtual void OnLeave(bool ret) override;
    virtual bool OnCallEventMessage(UIEventEnum e, const UIEvent::Event & param) override;

    bool OnEventKey(const UIEvent::Key & param);
    bool OnEventMenu(const UIEvent::Menu & param);
    bool OnEventMouse(const UIEvent::Mouse & param);

    glm::mat4 GetMatView();
    glm::mat4 GetMatProj();
    glm::mat4 GetMatViewProj();
    SharePtr<RawMesh> & GetMeshBuffer(size_t idx);
    bool HasOpMode(UIStateGLCanvas::Operation::OpModeEnum op);
    void AddOpMode(UIStateGLCanvas::Operation::OpModeEnum op, bool add);

    const SharePtr<GLObject> & GetProjectRoot();
    bool FromRectSelectObjects(const glm::vec2 & worldMin, 
                               const glm::vec2 & worldMax);
    void FromRectSelectObjects(const SharePtr<GLObject> & object, 
                               const glm::vec2 & local0, 
                               const glm::vec2 & local1, 
                               const glm::vec2 & local2, 
                               const glm::vec2 & local3, 
                               std::vector<SharePtr<GLObject>> & output);
    SharePtr<GLObject> FromCoordSelectObject(const SharePtr<GLObject> & object, const glm::vec2 & local);
    std::tuple<iint, SharePtr<Component>, glm::vec2, uint> FromCoordSelectTrackPoint(const glm::vec2 & screen);
};