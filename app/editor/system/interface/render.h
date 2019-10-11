#pragma once

#include "../../include.h"

class GLMesh;
class GLObject;
class GLProgram;
class GLTexture;
class GLMaterial;

namespace Interface {
    struct RenderCommand {
        std::function<void(const RenderCommand &)> mCallback;
        void Call() { if (mCallback) mCallback(*this); }
    };

    struct FowardCommand : public RenderCommand {
        SharePtr<GLMaterial> mMaterial;     //  ����
        glm::mat4 mTransform;               //  ����
    };

    struct PostCommand : public RenderCommand {
        enum TypeEnum {
            kOverlay,       //  ����
            kSwap,          //  ����
        };
        SharePtr<GLProgram> mProgram;       //  ��ɫ��
        SharePtr<GLMesh> mMesh;             //  ����
        glm::mat4 mTransform;               //  ����
        TypeEnum mType;
    };

    struct MatrixStack {
    public:
        enum TypeEnum {
            kModel,
            kView,
            kProj,
            Length,
        };

        void Pop(TypeEnum mode)
        {
            GetStack(mode).pop();
        }

        void Push(TypeEnum mode)
        {
            GetStack(mode).push(GetStack(mode).top());
        }

        void Identity(TypeEnum mode)
        {
            GetStack(mode).push(glm::mat4(1));
        }

        void Identity(TypeEnum mode, const glm::mat4 & mat)
        {
            GetStack(mode).push(mat);
        }

        void Mul(TypeEnum mode, const glm::mat4 & mat)
        {
            Push(mode);
            GetStack(mode).top() *= mat;
        }

        const glm::mat4 & Top(TypeEnum mode)
        {
            return GetStack(mode).top();
        }

        const glm::mat4 & GetM()
        {
            return Top(TypeEnum::kModel);
        }

        const glm::mat4 & GetV()
        {
            return Top(TypeEnum::kView);
        }

        const glm::mat4 & GetP()
        {
            return Top(TypeEnum::kProj);
        }

    private:
        std::stack<glm::mat4> & GetStack(TypeEnum mode)
        {
            return _matrixs[(size_t)mode];
        }

    private:
        std::stack<glm::mat4> _matrixs[Length];
    };
}