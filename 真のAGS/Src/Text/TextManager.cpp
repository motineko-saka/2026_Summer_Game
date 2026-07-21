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

	texts_.push_back("　ゲームプログラムとは、いやプログラムとは");
	texts_.push_back("ある事柄を実現するプログラムの方法を説明されても理解できないことがある。B");
	texts_.push_back("@　なぜならそのプログラム技法も何かの基本的な技法の組み合わせで出来ているからだ。B");
	texts_.push_back("@　これはその他の学問も基本がわからないと応用が利かないということと同じ現象で、");
	texts_.push_back("別に特に珍しいことでもない。B");
	texts_.push_back("C　しかしゲームプログラムとなると覚えなくてはならない基礎が沢山あり、");
	texts_.push_back("さらにある程度クオリティの高いソフトを作ろうとすると色々なプログラム技法を");
	texts_.push_back("習得しなくてはならない。B");
	texts_.push_back("@　しかもある程度レベルが高くなると自分で技法を編み出すか、技術レベルの高い");
	texts_.push_back("プログラマーに聞くなどするしか方法がなく大変厄介である。B"
		"というかそのせいでゲームプログラムの敷居は高くなっているといえる。BE");
}