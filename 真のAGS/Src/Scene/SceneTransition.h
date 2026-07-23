#pragma once
#include <vector>
#include <Windows.h>

class SceneTransition
{
public:

    enum class STATE
    {
        NONE,
        CLOSE,
        OPEN
    };

    SceneTransition();
    ~SceneTransition();

    void Start();
    void Open();

    void Update();
    void Draw();

    bool IsPlay() const;
    STATE GetState() const;

private:

    struct Piece
    {
        RECT rect;
        bool visible;
        int frame;
    };

    std::vector<Piece> pieces_;

    STATE state_;

    int timer_;
    bool isPlaying_;
};