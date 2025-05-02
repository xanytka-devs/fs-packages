#ifndef FS_UI_GENERIC
#define FS_UI_GENERIC

#include <glm/glm.hpp>

#include "../../../engine/include/common.hpp"
#include "../../../engine/include/shader.hpp"
#include "../../../engine/include/texture.hpp"
#include "../../../engine/include/utils/stbi_global.hpp"
#include "../../../engine/include/input/mouse.hpp"

namespace Firesteel {
	class Sprite {
	public:
		void initialize(const std::string tSprite = "") {
            // Load texture.
            if(tSprite != "" && std::filesystem::exists(tSprite)) {
                mTexture.ID = TextureFromFile(tSprite, &mTexture.isMonochrome, true);
                mTexture.path = tSprite.c_str();
                mHasTexture = true;
            }

            // Configure VAO and VBO.
            float vertices[] = {
            //   X     Y            UV
                0.0f, 1.0f,     0.0f, 1.0f,
                1.0f, 0.0f,     1.0f, 0.0f,
                0.0f, 0.0f,     0.0f, 0.0f,

                0.0f, 1.0f,     0.0f, 1.0f,
                1.0f, 1.0f,     1.0f, 1.0f,
                1.0f, 0.0f,     1.0f, 0.0f
            };

            glGenVertexArrays(1, &mVAO);
            glGenBuffers(1, &mVBO);

            glBindBuffer(GL_ARRAY_BUFFER, mVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindVertexArray(mVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
		}

		void draw(const Shader* tShader,
			glm::vec2 tProjectionSize=glm::vec2(0), glm::vec2 tPosition=glm::vec2(0), glm::vec2 tSize=glm::vec2(1), float tPitchRotation=0,
			glm::vec4 tColor=glm::vec4(1)) {
            // prepare transformations
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(tPosition.x, tProjectionSize.y-tPosition.y, 0.0f));
            model = glm::rotate(model, glm::radians(tPitchRotation), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(tSize, 1.0f));

            tShader->enable();
            tShader->setBool("hasTexture", mHasTexture);
            tShader->setBool("isFont", false);
            tShader->setMat4("model", model);
            tShader->setMat4("projection", glm::ortho(0.f, tProjectionSize.x, tProjectionSize.y, 0.f));
            tShader->setVec4("color", tColor);

            if(mHasTexture) mTexture.enable();
            glBindVertexArray(mVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
		}

        void remove() {
            glDeleteVertexArrays(1, &mVAO);
            glDeleteBuffers(1, &mVBO);
            if(mHasTexture) mTexture.remove();
            mHasTexture = false;
        }
	private:
		unsigned int mVAO = 0, mVBO = 0;
        bool mHasTexture = false;
        Texture mTexture;
	};

    class UIElement {
    public:
        virtual void onHover() {}
        virtual void onClick() {}

        void initialize(const std::string tSprite = "", const glm::vec2 tPosition = glm::vec2(0), const glm::vec2 tSize = glm::vec2(1), const float tPitch = 0) {
            mPos = tPosition;
            mSize = tSize;
            mPitch = tPitch;
            mSprite.initialize(tSprite);
        }

        void update(const glm::vec2 tProjectionSize) {
            glm::vec2 tCurPos = Mouse::getCursorPosition();
            if(tCurPos.x>= mPos.x && tCurPos.y>=(tProjectionSize.y-mPos.y) &&
                tCurPos.x <= mPos.x + mSize.x && tCurPos.y <= (tProjectionSize.y-mPos.y)+mSize.y) {
                mState=1;
                onHover();
                if(Mouse::buttonDown(0)) {
                    onClick();
                    mState=2;
                }
            } else mState=0;
        }
        void draw(const Shader* tShader, const glm::vec2 tProjectionSize) {
            glm::vec4 color = background;
            switch (mState) {
            case 1:
                color=hover;
                break;
            case 2:
                color=clicked;
                break;
            }
            mSprite.draw(tShader, tProjectionSize, mPos, mSize, mPitch, color);
        }
        void draw(const Shader* tShader,
            glm::vec2 tProjectionSize = glm::vec2(0), glm::vec2 tPosition = glm::vec2(0), glm::vec2 tSize = glm::vec2(1), float tPitch = 0,
            glm::vec4 tColor = glm::vec4(1)) {
            mSprite.draw(tShader, tProjectionSize, tPosition, tSize, tPitch, tColor);
        }

        void remove() {
            mSprite.remove();
        }

        glm::vec2 getPosition() const { return mPos; }
        float getPitch() const { return mPitch; }
        glm::vec2 getSize() const { return mSize; }

        void setPositon(glm::vec2 tPos) { mPos = tPos; }
        void setPitch(float tPitch) { mPitch = tPitch; }
        void setSize(glm::vec2 tSize) { mSize = tSize; }

        glm::vec4 background{0.25f}, hover{0.3f}, clicked{0.1f};
    protected:
        glm::vec2 mPos{0}, mSize{1};
        float mPitch=0;
        short mState=0;

        Sprite mSprite;
    };

    class Button : public UIElement {
    public:
        void setHoverCallback(void(*tFn)()) {
            onHoverCallback = tFn;
        }
        void setClickCallback(void(*tFn)()) {
            onClickCallback = tFn;
        }

        virtual void onHover() override {
            if(onHoverCallback) onHoverCallback();
        }
        virtual void onClick() override {
            if(onClickCallback) onClickCallback();
        }
    private:
        void(*onHoverCallback)() = nullptr;
        void(*onClickCallback)() = nullptr;
    };
}

#endif // !FS_UI_GENERIC