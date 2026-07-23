#include "PauseScene.h"
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Scene/TitleScene.h"
#include "../Audio/AudioManager.h"

PauseScene::PauseScene(void)
{
}
PauseScene::~PauseScene(void)
{
}

void PauseScene::Init()
{
    selectMenu_ = TITLE;
    SetMouseDispFlag(true);
}

void PauseScene::Load(void)
{
}
void PauseScene::LoadEnd(void) 
{
}

void PauseScene::Update(void)
{
    // そのまま戻る
    if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE) ||
        InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::OPTION))
    {
        SceneManager::GetInstance()->PopScene();
        return;
    }

    // 共通レイアウト値
    const int cx = Application::SCREEN_SIZE_X / 2;
    const int cy = Application::SCREEN_SIZE_Y / 2;
    const int vSpacing = 64;
    const int padW = 240;
    const int padH = 48;
    const int menuCount = 3;

    // ヘルパー：アイテム矩形を返す
    auto getItemRect = [&](int idx, int& left, int& top, int& right, int& bottom)
    {
        int itemY = cy + (idx - 1) * vSpacing;
        left = cx - padW / 2;
        right = cx + padW / 2;
        top = itemY - padH / 2;
        bottom = itemY + padH / 2;
    };

    // 選択を安全に循環させる
    auto wrapSelect = [&](int delta)
    {
        selectMenu_ = (selectMenu_ + delta) % menuCount;
        if (selectMenu_ < 0) selectMenu_ += menuCount;
    };

    // キーボード上下
    if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_UP))
    {
        wrapSelect(-1);
    }
    else if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_DOWN))
    {
        wrapSelect(+1);
    }

    // 左スティック
    {
        auto padState = InputManager::GetInstance()->GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
        float stickY = static_cast<float>(padState.AKeyLY) / InputManager::AKEY_VAL_MAX;
        static bool stickMoved = false;

        if (stickY < -InputManager::THRESHOLD && !stickMoved)
        {
            wrapSelect(-1);
            stickMoved = true;
        }
        else if (stickY > InputManager::THRESHOLD && !stickMoved)
        {
            wrapSelect(+1);
            stickMoved = true;
        }
        else if (fabs(stickY) < InputManager::THRESHOLD)
        {
            stickMoved = false;
        }
    }

    // マウスホバーで選択
    {
        auto mousePos = InputManager::GetInstance()->GetMousePos();
        for (int i = 0; i < menuCount; ++i)
        {
            int left, top, right, bottom;
            getItemRect(i, left, top, right, bottom);
            if (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom)
            {
                selectMenu_ = i;
                break;
            }
        }
    }

    // 選択確定で共通処理を実行
    auto ExecuteSelection = [&]() {
        switch (selectMenu_)
        {
        case 0: // タイトルへ戻る
            AudioManager::GetInstance()->StopBGM();
            SceneManager::GetInstance()->ChangeSceneTransition(std::make_shared<TitleScene>());
            break;
        case 1: // ゲーム終了
            SceneManager::GetInstance()->GameEnd();
            break;
        case 2: // 戻る
            SceneManager::GetInstance()->PopScene();
            break;
        default:
            break;
        }
        };

    // マウスクリックで決定（ホバーと同じ当たり判定を使用）
    if (InputManager::GetInstance()->IsTrgMouseLeft())
    {
        auto mousePos = InputManager::GetInstance()->GetMousePos();
        for (int i = 0; i < menuCount; ++i)
        {
            int left, top, right, bottom;
            getItemRect(i, left, top, right, bottom);
            if (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom)
            {
                selectMenu_ = i;
                ExecuteSelection();
                return;
            }
        }
    }

    // キー／パッドで決定
    if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_RETURN) ||
        InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE) ||
        InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
    {
        ExecuteSelection();
    }
}

void PauseScene::Draw(void)
{
    // 背景の半透明オーバーレイ
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 中央のパネル
    const int cx = Application::SCREEN_SIZE_X / 2;
    const int cy = Application::SCREEN_SIZE_Y / 2;
    const int padW = 240;
    const int padH = 48;
    const int vSpacing = 64;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
    DrawBox(cx - 240, cy - 160, cx + 240, cy + 160, 0xFFFFFF, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ヘッダーテキスト
    DrawFormatString(cx - 110, cy - 120, 0x000000, "ポーズメニュー");

    // メニュー項目
    const char* menuTexts[3] = { "タイトルへ戻る", "　ゲームを終了", "　戻る" };

    // 矩形取得を使って描画
    auto drawItemRect = [&](int i) {
        int itemY = cy + (i - 1) * vSpacing;
        int left = cx - padW / 2;
        int right = cx + padW / 2;
        int top = itemY - padH / 2;
        int bottom = itemY + padH / 2;

        if (selectMenu_ == i)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
            DrawBox(left, top, right, bottom, 0x444444, true);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            DrawFormatString(left + 16, itemY - 8, 0xFFFFFF, "%s", menuTexts[i]);
            DrawBox(left, top, right, bottom, 0x000000, false);
        }
        else
        {
            DrawFormatString(left + 16, itemY - 8, 0x000000, "%s", menuTexts[i]);
        }
    };

    for (int i = 0; i < 3; ++i) drawItemRect(i);
}

void PauseScene::Release(void) 
{
}