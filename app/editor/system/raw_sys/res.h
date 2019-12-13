#pragma once

#include "serializer.h"

class GLObject;

class Res : public Serializer {
public:
    enum TypeEnum {
        kNull,      //  无
        kTxt,       //  文本
        kImg,       //  图片
        kMap,       //  地图
        kFnt,       //  字体
        kObj,       //  对象
        kVar,       //  变量
        kBlueprint, //  蓝图
        Length,
    };

    static const std::string sTypeString[TypeEnum::Length];

    class Ptr {
    public:
        Ptr(Res* owner)
            : mModify(true)
            , mOwner(owner)
        { }

        ~Ptr()
        {
            if (mOwner != nullptr)
            {
                mOwner->DeletePtr(this);
            }
        }

        uint GetID()
        {
            return mOwner->GetID();
        }

        bool Valid()
        {
            return mOwner != nullptr;
        }

        void Invalid()
        {
            mOwner = nullptr;
        }

        bool Modify()
        {
            return mModify;
        }

        bool Modify(bool modify)
        {
            mModify=modify;
            return mModify;
        }

        Ptr * Clone()
        {
            return mOwner->AppendPtr();
        }

        std::string Path()
        {
            ASSERT_LOG(mOwner != nullptr, "");
            return mOwner->Path();
        }

        template <class T>
        SharePtr<T> Instance()
        {
            ASSERT_LOG(mOwner != nullptr, "");
            Modify(false);
            return mOwner->Instance<T>();
        }

    private:
        Res* mOwner;
        bool mModify;
    };

    class Ref : public Serializer {
    public:
        Ref(Ptr * owner): mOwner(owner)
        { }

        Ref(const Ref & other)
        {
            *this = other;
        }

        Ref(Ref && other)
        {
            *this = std::move(other);
        }

        Ref & operator=(Ref && other)
        {
            mValue.reset();
            mOwner.reset();
            mValue.swap(other.mValue);
            mOwner.swap(other.mOwner);
            return *this;
        }

        Ref & operator=(const Ref & other)
        {
            mValue     = other.mValue;
            mOwner.reset(other.mOwner->Clone());
            return *this; 
        }

        ~Ref()
        { }

        bool Vaild()
        {
            return mOwner != nullptr;
        }

        std::string Path()
        {
            ASSERT_LOG(Vaild(), "");
            return mOwner->Path();
        }

        template <class T>
        SharePtr<T> Instance()
        {
            ASSERT_LOG(Vaild(), "");
            if (mOwner->Modify())
            {
                mValue = mOwner->Instance<T>();
            }
            return std::any_cast<SharePtr<T>>(mValue);
        }

        virtual void EncodeBinary(std::ostream & os, Project * project) override;
        virtual void DecodeBinary(std::istream & is, Project * project) override;

    private:
        UniquePtr<Ptr> mOwner;
        std::any       mValue;
    };

public:
    Res(Project * owner, uint id = ~0);
    ~Res();

    //  实例化对象
    template <class T>
    SharePtr<T> Instance() { static_assert(false); }
    template <> SharePtr<GLObject> Instance();

    std::string Path();
    uint GetRefCount();
    uint GetID();

    void WakeRefs();
    Ptr * AppendPtr();
    Ref   AppendRef();
    void  DeletePtr(Ptr * ptr);

    TypeEnum Type();
    TypeEnum Type(TypeEnum type);

    const        std::string & TypeString();
    static const std::string & TypeString(uint type);

    template <class T>
    void BindMeta(const T & val)
    {
        if (!mMeta.has_value() || std::any_cast<T &>(mMeta) != val)
        {
            mMeta = val; WakeRefs();
        }
    }

    virtual void EncodeBinary(std::ostream & os, Project * project) override;
    virtual void DecodeBinary(std::istream & is, Project * project) override;

private:
    uint                mID;
    TypeEnum            mType;      //  类型
    std::any            mMeta;      //  元数据
    std::vector<Ptr *>  mPtrs;      //  引用列表
    Project *           mOwner;     //  项目归属
};