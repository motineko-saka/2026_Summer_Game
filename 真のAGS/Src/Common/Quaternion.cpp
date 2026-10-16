#include <cmath>
#include <algorithm>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "Quaternion.h"

namespace
{
	// これ以上の角度は「ほぼ真逆」とみなす(度)
	constexpr double kOppositeAngleDeg = 179.9196;

	// 回転軸として扱える最小の長さ(2乗)
	constexpr float kMinAxisSqrLen = 0.000001f;

	// これ以上 cos が大きい(角度が小さい)場合は線形補間で済ませる
	constexpr float kLerpThreshold = 0.99f;

	// ToAngleAxis で軸を求められる最小の分母
	constexpr float kMinAxisDen = 0.0001f;
}

// ---------------------------------------------------------------
// コンストラクタ
// ---------------------------------------------------------------
Quaternion::Quaternion(void)
	: w(1.0), x(0.0), y(0.0), z(0.0)
{
}

Quaternion::Quaternion(const VECTOR& rad)
	: Quaternion(Euler(rad))
{
}

Quaternion::Quaternion(double qw, double qx, double qy, double qz)
	: w(qw), x(qx), y(qy), z(qz)
{
}

// ---------------------------------------------------------------
// 生成・変換
// ---------------------------------------------------------------
Quaternion Quaternion::Euler(const VECTOR& rad)
{
	return Euler(rad.x, rad.y, rad.z);
}

Quaternion Quaternion::Euler(double radX, double radY, double radZ)
{
	radX = AsoUtility::RadIn2PI(radX);
	radY = AsoUtility::RadIn2PI(radY);
	radZ = AsoUtility::RadIn2PI(radZ);

	const double cosX = std::cos(radX * 0.5);
	const double sinX = std::sin(radX * 0.5);
	const double cosY = std::cos(radY * 0.5);
	const double sinY = std::sin(radY * 0.5);
	const double cosZ = std::cos(radZ * 0.5);
	const double sinZ = std::sin(radZ * 0.5);

	return Quaternion(
		cosX * cosY * cosZ + sinX * sinY * sinZ,
		sinX * cosY * cosZ + cosX * sinY * sinZ,
		cosX * sinY * cosZ - sinX * cosY * sinZ,
		cosX * cosY * sinZ - sinX * sinY * cosZ);
}

Quaternion Quaternion::AngleAxis(double rad, VECTOR axis)
{
	const double sqrLen =
		static_cast<double>(axis.x) * axis.x +
		static_cast<double>(axis.y) * axis.y +
		static_cast<double>(axis.z) * axis.z;

	// 軸が無ければ回転なし
	if (sqrLen <= 0.0) return Identity();

	// 軸を正規化（元実装と同じく float に丸める）
	const double invLen = 1.0 / std::sqrt(sqrLen);
	const float ax = static_cast<float>(axis.x * invLen);
	const float ay = static_cast<float>(axis.y * invLen);
	const float az = static_cast<float>(axis.z * invLen);

	const double half = 0.5 * rad;
	const double s = std::sin(half);

	return Quaternion(std::cos(half), s * ax, s * ay, s * az);
}

VECTOR Quaternion::ToEuler(const Quaternion& q)
{
	const double r11 = 2.0 * (q.x * q.z + q.w * q.y);
	const double r12 = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
	const double r21 = -2.0 * (q.y * q.z - q.w * q.x);
	const double r31 = 2.0 * (q.x * q.y + q.w * q.z);
	const double r32 = q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z;

	return VGet(
		static_cast<float>(std::asin(r21)),
		static_cast<float>(std::atan2(r11, r12)),
		static_cast<float>(std::atan2(r31, r32)));
}

VECTOR Quaternion::ToEuler(void) const
{
	return ToEuler(*this);
}

MATRIX Quaternion::ToMatrix(const Quaternion& q)
{
	const float fx = static_cast<float>(q.x);
	const float fy = static_cast<float>(q.y);
	const float fz = static_cast<float>(q.z);
	const float fw = static_cast<float>(q.w);

	const float xx = fx * fx * 2.0f;
	const float yy = fy * fy * 2.0f;
	const float zz = fz * fz * 2.0f;
	const float yz = fy * fz * 2.0f;
	const float xz = fx * fz * 2.0f;
	const float xy = fx * fy * 2.0f;
	const float wx = fw * fx * 2.0f;
	const float wy = fw * fy * 2.0f;
	const float wz = fw * fz * 2.0f;

	MATRIX mat = MGetIdent();
	mat.m[0][0] = 1.0f - (yy + zz);	mat.m[0][1] = xy + wz;			mat.m[0][2] = xz - wy;
	mat.m[1][0] = xy - wz;			mat.m[1][1] = 1.0f - (xx + zz);	mat.m[1][2] = yz + wx;
	mat.m[2][0] = xz + wy;			mat.m[2][1] = yz - wx;			mat.m[2][2] = 1.0f - (xx + yy);

	return mat;
}

MATRIX Quaternion::ToMatrix(void) const
{
	return ToMatrix(*this);
}

Quaternion Quaternion::LookRotation(const VECTOR& dir)
{
	return LookRotation(dir, VGet(0.0f, 1.0f, 0.0f));
}

Quaternion Quaternion::LookRotation(const VECTOR& dir, const VECTOR& up)
{
	// 右・上・前 の3軸から回転行列を作り、行列→クォータニオン変換に任せる
	const VECTOR forward = AsoUtility::VNormalize(dir);
	const VECTOR right = AsoUtility::VNormalize(VCross(up, forward));
	const VECTOR upward = VCross(forward, right);

	MATRIX mat = MGetIdent();
	mat.m[0][0] = right.x;		mat.m[0][1] = right.y;		mat.m[0][2] = right.z;
	mat.m[1][0] = upward.x;		mat.m[1][1] = upward.y;		mat.m[1][2] = upward.z;
	mat.m[2][0] = forward.x;	mat.m[2][1] = forward.y;	mat.m[2][2] = forward.z;

	return GetRotation(mat).Normalized();
}

Quaternion Quaternion::GetRotation(const MATRIX& mat)
{
	const auto& m = mat.m;

	// 対角成分の和が十分大きければ w を基準に求める
	const float tr = m[0][0] + m[1][1] + m[2][2] + 1.0f;
	if (tr >= 1.0f)
	{
		const float s = 0.5f / sqrtf(tr);
		return Quaternion(
			0.25f / s,
			(m[1][2] - m[2][1]) * s,
			(m[2][0] - m[0][2]) * s,
			(m[0][1] - m[1][0]) * s);
	}

	// それ以外は対角成分が最大の軸を基準にする（精度確保のため）
	const float maxYZ = (m[1][1] > m[2][2]) ? m[1][1] : m[2][2];

	if (maxYZ < m[0][0])
	{
		const float r = sqrtf(m[0][0] - (m[1][1] + m[2][2]) + 1.0f);
		const float s = 0.5f / r;
		return Quaternion(
			(m[1][2] - m[2][1]) * s,
			r * 0.5f,
			(m[0][1] + m[1][0]) * s,
			(m[2][0] + m[0][2]) * s);
	}

	if (maxYZ == m[1][1])
	{
		const float r = sqrtf(m[1][1] - (m[2][2] + m[0][0]) + 1.0f);
		const float s = 0.5f / r;
		return Quaternion(
			(m[2][0] - m[0][2]) * s,
			(m[0][1] + m[1][0]) * s,
			r * 0.5f,
			(m[1][2] + m[2][1]) * s);
	}

	const float r = sqrtf(m[2][2] - (m[0][0] + m[1][1]) + 1.0f);
	const float s = 0.5f / r;
	return Quaternion(
		(m[0][1] - m[1][0]) * s,
		(m[2][0] + m[0][2]) * s,
		(m[1][2] + m[2][1]) * s,
		r * 0.5f);
}

Quaternion Quaternion::FromToRotation(const VECTOR& fromDir, const VECTOR& toDir)
{
	VECTOR axis = VCross(fromDir, toDir);
	const double angleDeg = AsoUtility::AngleDeg(fromDir, toDir);

	// ほぼ真逆だと外積が0になるため、垂直な軸を作り直す
	if (angleDeg >= kOppositeAngleDeg)
	{
		const VECTOR r = VCross(fromDir, AsoUtility::DIR_R);
		axis = VCross(r, fromDir);
		if (VSquareSize(axis) < kMinAxisSqrLen)
		{
			axis = AsoUtility::DIR_U;
		}
	}

	// AngleAxis 内で正規化される
	return AngleAxis(AsoUtility::Deg2RadD(angleDeg), axis);
}

Quaternion Quaternion::Identity(void)
{
	return Quaternion();
}

// ---------------------------------------------------------------
// 演算
// ---------------------------------------------------------------
Quaternion Quaternion::Mult(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
		q1.w * q2.x + q2.w * q1.x + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q2.w * q1.y + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q2.w * q1.z + q1.x * q2.y - q1.y * q2.x);
}

Quaternion Quaternion::Mult(const Quaternion& q) const
{
	return Mult(*this, q);
}

VECTOR Quaternion::PosAxis(const Quaternion& q, VECTOR pos)
{
	// pos' = q・pos・q^-1
	const Quaternion p(0.0, pos.x, pos.y, pos.z);
	const Quaternion ret = q.Mult(p).Mult(q.Inverse());
	return ret.xyz();
}

VECTOR Quaternion::PosAxis(VECTOR pos) const
{
	return PosAxis(*this, pos);
}

double Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

double Quaternion::Dot(const Quaternion& q) const
{
	return Dot(*this, q);
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
	return q.Normalized();
}

Quaternion Quaternion::Normalized(void) const
{
	const double len = Length();
	return Quaternion(w / len, x / len, y / len, z / len);
}

void Quaternion::Normalize(void)
{
	*this = Normalized();
}

Quaternion Quaternion::Inverse(void) const
{
	const double inv = 1.0 / LengthSquared();
	return Quaternion(w * inv, -x * inv, -y * inv, -z * inv);
}

double Quaternion::Length(void) const
{
	return std::sqrt(LengthSquared());
}

double Quaternion::LengthSquared(void) const
{
	return w * w + x * x + y * y + z * z;
}

VECTOR Quaternion::xyz(void) const
{
	return VGet(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

// ---------------------------------------------------------------
// 補間・角度
// ---------------------------------------------------------------
Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, double t)
{
	return SlerpUnclamped(from, to, static_cast<float>(std::clamp(t, 0.0, 1.0)));
}

Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t)
{
	// どちらかが0なら、もう一方を返す
	const bool aIsZero = (a.LengthSquared() == 0.0);
	const bool bIsZero = (b.LengthSquared() == 0.0);
	if (aIsZero) return bIsZero ? Identity() : b;
	if (bIsZero) return a;

	float cosHalf = static_cast<float>(a.w * b.w) + VDot(a.xyz(), b.xyz());

	// 角度0なので補間不要
	if (cosHalf >= 1.0f || cosHalf <= -1.0f) return a;

	// 最短経路で補間するため b を反転
	if (cosHalf < 0.0f)
	{
		b = Quaternion(-b.w, -b.x, -b.y, -b.z);
		cosHalf = -cosHalf;
	}

	float blendA;
	float blendB;
	if (cosHalf < kLerpThreshold)
	{
		// 角度が大きい場合は球面補間
		const float halfAngle = acosf(cosHalf);
		const float invSin = 1.0f / sinf(halfAngle);
		blendA = sinf(halfAngle * (1.0f - t)) * invSin;
		blendB = sinf(halfAngle * t) * invSin;
	}
	else
	{
		// 角度が小さい場合は線形補間
		blendA = 1.0f - t;
		blendB = t;
	}

	const VECTOR v = VAdd(VScale(a.xyz(), blendA), VScale(b.xyz(), blendB));
	const Quaternion result(blendA * a.w + blendB * b.w, v.x, v.y, v.z);

	return (result.LengthSquared() > 0.0) ? result.Normalized() : Identity();
}

Quaternion Quaternion::RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta)
{
	const double angle = Angle(from, to);
	if (angle == 0.0) return to;

	// windows.h の min マクロを避けるため括弧で囲む
	const float t = (std::min)(1.0f, maxDegreesDelta / static_cast<float>(angle));
	return SlerpUnclamped(from, to, t);
}

double Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
{
	// 誤差で ±1 を超えると acos が NaN になるため丸める
	const double dot = std::clamp(Dot(q1, q2), -1.0, 1.0);
	return std::acos(dot) * (180.0 / DX_PI);
}

void Quaternion::ToAngleAxis(float* angle, VECTOR* axis)
{
	if (std::abs(w) > 1.0) Normalize();

	const bool noRotation = (x == 0.0 && y == 0.0 && z == 0.0);
	*angle = noRotation ? 0.0f : 2.0f * acosf(static_cast<float>(w));

	const float den = sqrtf(1.0f - static_cast<float>(w * w));
	if (den > kMinAxisDen)
	{
		const VECTOR v = xyz();
		*axis = VGet(v.x / den, v.y / den, v.z / den);
	}
	else
	{
		// 角度0のときは軸が決まらないので適当な単位ベクトル
		*axis = VGet(1.0f, 0.0f, 0.0f);
	}
}

// ---------------------------------------------------------------
// 基本ベクトル
// ---------------------------------------------------------------
VECTOR Quaternion::GetDir(VECTOR dir) const { return PosAxis(dir); }

VECTOR Quaternion::GetForward(void) const { return GetDir(AsoUtility::DIR_F); }
VECTOR Quaternion::GetBack(void) const { return GetDir(AsoUtility::DIR_B); }
VECTOR Quaternion::GetRight(void) const { return GetDir(AsoUtility::DIR_R); }
VECTOR Quaternion::GetLeft(void) const { return GetDir(AsoUtility::DIR_L); }
VECTOR Quaternion::GetUp(void) const { return GetDir(AsoUtility::DIR_U); }
VECTOR Quaternion::GetDown(void) const { return GetDir(AsoUtility::DIR_D); }