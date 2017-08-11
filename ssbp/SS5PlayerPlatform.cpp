// 
//  SS5Platform.cpp
//
#include "SS5PlayerPlatform.h"
#include "../texture.h"
#include "../quad.h"

/**
* 各プラットフォームに合わせて処理を作成してください
* DXライブラリ用に作成されています。
*/
#include <iostream>
#include "../sprite.h"

extern Sprite sprite;

namespace ss
{
	/**
	* ファイル読み込み
	*/
	unsigned char* SSFileOpen(const char* pszFileName, const char* pszMode, unsigned long * pSize)
	{
		unsigned char * pBuffer = NULL;
		SS_ASSERT2(pszFileName != NULL && pSize != NULL && pszMode != NULL, "Invalid parameters.");
		*pSize = 0;
		do
		{
			// read the file from hardware
			FILE *fp = fopen(pszFileName, pszMode);
			std::fstream fps{ pszFileName };
			SS_BREAK_IF(!fp);

			fseek(fp, 0, SEEK_END);
			*pSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			pBuffer = new unsigned char[*pSize];
			*pSize = fread(pBuffer, sizeof(unsigned char), *pSize, fp);
			fclose(fp);
		} while (0);
		if (!pBuffer)
		{
			std::string msg = "Get data from file(";
			msg.append(pszFileName).append(") failed!");
			SSLOG("%s", msg.c_str());
		}
		return pBuffer;
	}

	/**
	* テクスチャの読み込み
	*/
	long SSTextureLoad(const char* pszFileName, SsTexWrapMode::_enum  wrapmode, SsTexFilterMode::_enum filtermode)
	{
		/**
		* テクスチャ管理用のユニークな値を返してください。
		* テクスチャの管理はゲーム側で行う形になります。
		* テクスチャにアクセスするハンドルや、テクスチャを割り当てたバッファ番号等になります。
		*
		* プレイヤーはここで返した値とパーツのステータスを引数に描画を行います。
		*/
		static long rc = 0;
		sprite.textures[rc++] = new Texture(pszFileName);

		//SpriteStudioで設定されたテクスチャ設定を反映させるための分岐です。
		switch (wrapmode)
		{
		case SsTexWrapMode::clamp:	//クランプ
									//std::cout << "CLAMP\n";
			break;
		case SsTexWrapMode::repeat:	//リピート
									//std::cout << "REPEAT\n";
			break;
		case SsTexWrapMode::mirror:	//ミラー
									//std::cout << "MIRROR\n";
			break;
		}
		switch (filtermode)
		{
		case SsTexFilterMode::nearlest:	//ニアレストネイバー
										//std::cout << "NEAREST NEIGHBOR\n";
			break;
		case SsTexFilterMode::linear:	//リニア、バイリニア
										//std::cout << "BILINEAR\n";
			break;
		}

		return rc;
	}

	/**
	* テクスチャの解放
	*/
	bool SSTextureRelese(long handle)
	{
		/// 解放後も同じ番号で何度も解放処理が呼ばれるので、例外が出ないように作成してください。
		bool rc = true;
		if (handle == -1)
		{
			rc = false;
		}
		delete sprite.textures[handle - 1];
		sprite.textures[handle - 1] = nullptr;
		return rc;
	}

	/**
	* テクスチャのサイズを取得
	* テクスチャのUVを設定するのに使用します。
	*/
	bool SSGetTextureSize(long handle, int &w, int &h)
	{
		w = sprite.textures[handle - 1]->width;
		h = sprite.textures[handle - 1]->height;
		return true;
	}
	/**
	* スプライトの表示
	*/
	void SSDrawSprite(State state)
	{
		// TEXTURE COORDINATES
		float u_scale = state.quad.tr.texCoords.u - state.quad.bl.texCoords.u;
		float v_scale = state.quad.bl.texCoords.v - state.quad.tr.texCoords.v;
		float u_offset = state.quad.bl.texCoords.u;
		float v_offset = 1.0 - state.quad.bl.texCoords.v;
		glm::vec4 uv_offset(u_scale, v_scale, u_offset, v_offset);
		sprite.shader.setVec4v("u_ImageOffset", glm::value_ptr(uv_offset), 1);
		// TEXTURE
		sprite.shader.setTexture2D("u_Texture", sprite.textures[state.texture.handle - 1]->id);

		const float fivTwel = 1.0f / 256.0f;
		// VERTEX TRANSFORMATIONS
		glm::mat4 mat = glm::make_mat4(state.mat);
		mat = glm::rotate(mat, glm::radians(-state.instancerotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
		sprite.shader.setMat4("u_Transform", glm::value_ptr(mat));
		sprite.shader.setFloat("u_Opacity", state.opacity / 255.0f);
		sprite.shader.setBool("u_UseTexture", sprite.textures[state.texture.handle - 1]->loaded);

		// VERTICES
		float quad[12] {
			state.quad.tr.vertices.x, state.quad.tr.vertices.y, state.quad.tr.vertices.z,
			state.quad.br.vertices.x, state.quad.br.vertices.y, state.quad.br.vertices.z,
			state.quad.bl.vertices.x, state.quad.bl.vertices.y, state.quad.bl.vertices.z,
			state.quad.tl.vertices.x, state.quad.tl.vertices.y, state.quad.tl.vertices.z
		};

		sprite.shader.setVec3v("u_Quad", quad, 12);
		sprite.render_quad();
	}

	/**
	* ユーザーデータの取得
	*/
	void SSonUserData(Player *player, UserData *userData)
	{
		//ゲーム側へユーザーデータを設定する関数を呼び出してください。
	}

	/**
	* ユーザーデータの取得
	*/
	void SSPlayEnd(Player *player)
	{
		//ゲーム側へアニメ再生終了を設定する関数を呼び出してください。
	}

	/**
	* windows用パスチェック
	*/
	bool isAbsolutePath(const std::string& strPath)
	{
		std::string strPathAscii = strPath.c_str();
		if (strPathAscii.length() > 2
			&& ((strPathAscii[0] >= 'a' && strPathAscii[0] <= 'z') || (strPathAscii[0] >= 'A' && strPathAscii[0] <= 'Z'))
			&& strPathAscii[1] == ':')
		{
			return true;
		}
		return false;
	}

};
