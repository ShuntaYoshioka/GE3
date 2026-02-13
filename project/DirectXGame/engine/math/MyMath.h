#pragma once
#include <engine/math/Vector2.h>
#include <engine/math/Vector3.h>
#include <engine/math/Vector4.h>
#include <engine/math/Matrix4x4.h>

class MyMath
{
public:
	// 単位行列
	static	Matrix4x4 MakeIdentity4x4();
	// 4x4の掛け算
	static	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	// X軸で回転
	static	Matrix4x4 MakeRotateXMatrix(float radian);
	// Y軸で回転
	static	Matrix4x4 MakeRotateYMatrix(float radian);
	// Z軸で回転
	static	Matrix4x4 MakeRotateZMatrix(float radian);
	// Affine変換
	static	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	// 透視投影行列
	static	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	// 逆行列
	static	Matrix4x4 Inverse(const Matrix4x4& m);
	// 正射影行列
	static	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
};

