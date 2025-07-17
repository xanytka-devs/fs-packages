#ifndef FS_UI_TEXT
#define FS_UI_TEXT

#include <filesystem>
#include <string>
#include <map>
#include <glm/ext/matrix_clip_space.hpp>
#include <../external/freetype/ft2build.h>
#include FT_FREETYPE_H
#include <../external/freetype/include/freetype.h>

#include "../../../engine/include/common.hpp"
#include "../../../engine/include/shader.hpp"

FT_Library gFreeType;
bool gInit;
bool gDone;

namespace Firesteel {
	typedef struct {
		glm::ivec2 size;			// Size of character.
		glm::ivec2 bearing;			// Distance from origin to top left of character.
		unsigned int advance;		// Distance from origin to next origin (1/64th pixels).
		glm::vec2 topLeft;			// Location from top left. [0,0]
		glm::vec2 bottomRight;		// Location from bottom right. [1,1]
	} Character;

	class TextRenderer {
	public:
		static void initialize() {
			if(FT_Init_FreeType(&gFreeType)) {
				LOG_ERRR("Couldn't load FreeType library.");
				return;
			}
			gInit = true;
		}
		static bool isInitialized() {
			return gInit;
		}
	};

	// [!WARNING]
	// This enum provides basic glyph ranges (mostly latin+cyrillic).
	enum TextGlyphRange {
		TGR_ASCII=128,
		TGR_KOI8_R=256,
		TGR_ISO8859_5=242
	};

	class Text {
	public:
		bool loadFont(const std::string tTTFPath, const int tHeight, const TextGlyphRange tLastCharId=TGR_ASCII) {
			if(!TextRenderer::isInitialized()) return false;
			if(!std::filesystem::exists(tTTFPath)) {
				LOG_ERRR("Font file \"" + tTTFPath + "\" doesn't exist.");
				return false;
			}
			//Assign variables and load font.
			mChars.clear();
			mHeight = tHeight;
			FT_Face font;
			if (FT_New_Face(gFreeType, tTTFPath.c_str(), 0, &font)) {
				LOG_ERRR("Couldn't load font file \"" + tTTFPath + "\".");
				return false;
			}
			//Set height and dynamic width.
			FT_Set_Pixel_Sizes(font, 0, mHeight);
			//Load texture.
			//Disables the byte-alignment restriction so can use 1 byte for each pixel.
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			int atlasWidth = 0, atlasHeight = 0;
			//Load characters of given glyph array.
			for (unsigned char c = 0; c < tLastCharId; c++) {
				//Load glyph.
				if (FT_Load_Char(font, c, FT_LOAD_RENDER)) {
					LOG_ERRR(std::string("Couldn't load glyph #") + (const char *)(c+1) + ".");
					continue;
				}
				//Update atlas variables.
				atlasWidth += font->glyph->bitmap.width;
				if(atlasHeight<static_cast<int>(font->glyph->bitmap.rows))
					atlasHeight = static_cast<int>(font->glyph->bitmap.rows);
			}
			//Generate texture.
			glGenTextures(1, &mTextureID);
			glBindTexture(GL_TEXTURE_2D, mTextureID);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				atlasWidth,
				atlasHeight,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				nullptr
			);
			//Set texture parameters.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//Place glyphs into atlas.
			int x = 0;
			for (unsigned char c = 0; c < tLastCharId; c++) {
				//Load glyph.
				if(FT_Load_Char(font, static_cast<char>(c), FT_LOAD_RENDER)) {
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				//Add glyph to texture atlas.
				glTextureSubImage2D(
					mTextureID,
					0,
					x,
					0,
					font->glyph->bitmap.width,
					font->glyph->bitmap.rows,
					GL_RED,
					GL_UNSIGNED_BYTE,
					font->glyph->bitmap.buffer
				);
				//Store glyph information in character map.
				glm::ivec2 Size = glm::ivec2(font->glyph->bitmap.width, font->glyph->bitmap.rows);
				Character character{
					Size,
					glm::ivec2(font->glyph->bitmap_left, font->glyph->bitmap_top),
					static_cast<unsigned int>(font->glyph->advance.x),

					glm::vec2(static_cast<float>(x) / static_cast<float>(atlasWidth),0.0f),
					glm::vec2(static_cast<float>(x + Size.x) / static_cast<float>(atlasWidth), static_cast<float>(Size.y) / static_cast<float>(atlasHeight))
				};
				mChars.insert(std::pair<char, Character>(c, character));

				// update x position for next glyph
				x += font->glyph->bitmap.width;
			}
			//Signal that font was loaded.
			glBindTexture(GL_TEXTURE_2D, 0);
			FT_Done_Face(font);
			//Config VAO and VBO.
			glGenVertexArrays(1, &mTextVAO);
			glGenBuffers(1, &mTextVBO);
			glBindVertexArray(mTextVAO);
			glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			return true;
		}

		void draw(Shader* tShader, std::string tText, glm::vec2 tProjectionSize, glm::vec2 tPosition, glm::vec2 tSize, glm::vec4 tColor) {
			draw(tShader,tText,tProjectionSize,glm::vec3(tPosition,0),tSize,tColor);
		}

		void draw(Shader* tShader, std::string tText, glm::vec2 tProjectionSize, glm::vec3 tPosition, glm::vec2 tSize, glm::vec4 tColor) {
			if (!TextRenderer::isInitialized()) return;
			if (!gDone) {
				FT_Done_FreeType(gFreeType);
				gDone = true;
			}
			//Setup shader.
			tShader->enable();
			tShader->setBool("isFont", true);
			tShader->setBool("hasTexture", true);
			tShader->setVec4("color", tColor);
			tShader->setMat4("projection", glm::ortho(0.f, tProjectionSize.x, 0.f, tProjectionSize.y));
			tShader->setMat4("model", glm::translate(glm::mat4(1), glm::vec3(0,0,tPosition.z)));
			//Setup render data.
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(mTextVAO);
			glBindTexture(GL_TEXTURE_2D, mTextureID);
			glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
			//Setup advancable data.
			Character c{};
			float xpos, ypos; // characters might need to be shifted below baseline
			float w = c.size.x * tSize.x, h = c.size.y * tSize.y;
			//Go through all characters of string.
			for (size_t i = 0, len = tText.size(); i < len; i++) {
				//Advance data.
				c = mChars[tText[i]];
				xpos = tPosition.x + c.bearing.x * tSize.x;
				ypos = tPosition.y - (c.size.y - c.bearing.y) * tSize.y;
				w = c.size.x * tSize.x, h = c.size.y * tSize.y;
				//New VBO data.
				glm::vec2 b = c.bottomRight, t = c.topLeft;
				float vertices[6][4] = {
					//		X		  Y				UV
						{ xpos,     ypos + h,   t.x, t.y },
						{ xpos,     ypos,       t.x, b.y },
						{ xpos + w, ypos,       b.x, b.y },
			
						{ xpos,     ypos + h,   t.x, t.y },
						{ xpos + w, ypos,       b.x, b.y },
						{ xpos + w, ypos + h,   b.x, t.y }
				};
				//Render quad.
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				//Advance cursor.
				tPosition.x += (c.advance >> 6) * tSize.x; // multiply by 64 
			}
			//Clear bindings.
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void remove() {
			if(!TextRenderer::isInitialized()) return;
			glDeleteVertexArrays(1, &mTextVAO);
			glDeleteBuffers(1, &mTextVBO);
			mChars.clear();
		}

		~Text() { remove(); }
	private:
		int mHeight = 0;
		std::map<char, Character> mChars;
		unsigned int mTextVAO = 0, mTextVBO = 0, mTextureID = 0;
	};

}

#endif // !FS_UI_TEXT