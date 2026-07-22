#include "TextManager.h"
#include <fstream>

TextManager* TextManager::instance_ = nullptr;

TextManager::TextManager()
	: texts_()
	, sp_(0)
	, cp_(0)
{
}

TextManager::~TextManager()
{
}

void TextManager::Init()
{
	LoadDefaultText();
	sp_ = 0;
	cp_ = 0;
}

bool TextManager::LoadFromFile(const std::string& path)
{
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return false;

	texts_.clear();
	std::string line;
	// 単純に1行ずつ読み込む（改行は行境界として扱う）
	while (std::getline(ifs, line))
	{
		texts_.push_back(line);
	}
	sp_ = 0;
	cp_ = 0;
	return true;
}

char TextManager::CurrentByte() const
{
	if (sp_ >= texts_.size()) return '\0';
	const std::string& s = texts_[sp_];
	if (cp_ >= s.size()) return '\0';
	return s[cp_];
}

void TextManager::GetCurrentTwoByte(char out[3]) const
{
	out[0] = '\0';
	out[1] = '\0';
	out[2] = '\0';

	if (sp_ >= texts_.size()) return;
	const std::string& s = texts_[sp_];
	if (cp_ >= s.size()) return;

	out[0] = s[cp_];
	if (cp_ + 1 < s.size()) out[1] = s[cp_ + 1];
	else out[1] = '\0';
	out[2] = '\0';
}

void TextManager::AdvanceBytes(int n)
{
	if (sp_ >= texts_.size()) return;

	// 進める
	cp_ += static_cast<size_t>(n);

	// 行を跨いだら次行へ（複数行進む可能性を考慮）
	while (sp_ < texts_.size() && cp_ >= texts_[sp_].size())
	{
		sp_++;
		cp_ = 0;
	}
}

bool TextManager::IsAllTextConsumed() const
{
	return sp_ >= texts_.size();
}

void TextManager::Reset()
{
	sp_ = 0;
	cp_ = 0;
}

void TextManager::LoadDefaultText()
{
	texts_.clear();

	texts_.push_back("こ、ここはいったいどこなんだ？");
	texts_.push_back("僕は全く見覚えのないところに立っていた。B");
	texts_.push_back("@　意識の混濁が激しい。B");
	texts_.push_back("@　さっき起こったことを一つずつ思い出していこう。");
	texts_.push_back("それは遡ること数十分前。B");
	texts_.push_back("C　いつものように僕は図書館で本を探していた。");
	texts_.push_back("その中にひときわ目立つ本があったので手に取ってみたらなんと本の中に吸い込まれてしまった。");
	texts_.push_back("そんな感じで今僕は知らない世界にいる。B");
	texts_.push_back("@　そうして辺りを見渡していると反対側に誰かいるのが見えた。");
	texts_.push_back("プどうやらここは謎の壁があって反対側はいけないらしい。。B"
		"そんな中僕は反対側にいる人物に声をかけた。BE");
}