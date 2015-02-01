#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <cmath>
using std::ostream;
using std::istream;

/////////////////////////////////////////////////
const float PI = 3.141592654f;
const float QTR_PI = PI / 4.0f;
const float HALF_PI = PI / 2.0f;
const float TWO_PI = 2.0f * PI;

//const float ROOT2DIV2 = 0.7071067812;

/////////////////////////////////////////////////
struct sVector2d
{
	float m_x, m_y;

	sVector2d();
	sVector2d(float, float);
	sVector2d(const sVector2d&);

	void Set(float, float);
	const sVector2d& operator=(const sVector2d&);

	bool operator==(const sVector2d&);
	bool operator!=(const sVector2d&);

	sVector2d operator+(const sVector2d&);
	sVector2d operator-(const sVector2d&);
	const sVector2d& operator+=(const sVector2d&);
	const sVector2d& operator-=(const sVector2d&);

	sVector2d operator*(float);
	sVector2d operator/(float);
	const sVector2d& operator*=(float);
	const sVector2d& operator/=(float);
	float Dot(const sVector2d&);

	float Length();
	float NormSquared();

	sVector2d Normalize();
};

sVector2d operator*(float, const sVector2d&);
sVector2d operator/(float, const sVector2d&);

/////////////////////////////////////////////////
struct sVector3d
{
	float m_x, m_y, m_z;

	sVector3d ();
	sVector3d (float, float, float);
	sVector3d (const sVector3d&);

	void Set (float, float, float);
	const sVector3d& operator= (const sVector3d&);

	bool operator== (const sVector3d&);
	bool operator!= (const sVector3d&);

	sVector3d operator+ (const sVector3d&) const;
	sVector3d operator- (const sVector3d&) const;
	const sVector3d& operator+= (const sVector3d&);
	const sVector3d& operator-= (const sVector3d&);

	sVector3d operator* (float);
	sVector3d operator/ (float);
	const sVector3d& operator*= (float);
	const sVector3d& operator/= (float);
	sVector3d operator* (const sVector3d&);
	sVector3d operator/ (const sVector3d&);
	const sVector3d& operator*= (const sVector3d&);
	const sVector3d& operator/= (const sVector3d&);
	float Dot (const sVector3d&);
	sVector3d Cross (const sVector3d&);

	float Length ();
	float NormSquared ();

	sVector3d Normalize ();

};

/////////////////////////////////////////////////
struct sPoint
{
public:
	int m_x, m_y;

	sPoint ();
	sPoint (int x, int y);

	void Set (int x, int y);

	float Distance (const sPoint&);

	bool operator== (const sPoint&);
	bool operator!= (const sPoint&);

	sPoint operator+ (int);
	sPoint operator- (int);
	sPoint operator* (int);
	sPoint operator/ (int);

	sPoint& operator+= (int);
	sPoint& operator-= (int);
	sPoint& operator*= (int);
	sPoint& operator/= (int);

	sPoint operator+ (const sPoint&);
	sPoint operator- (const sPoint&);
	sPoint operator* (const sPoint&);
	sPoint operator/ (const sPoint&);

	sPoint& operator+= (const sPoint&);
	sPoint& operator-= (const sPoint&);
	sPoint& operator*= (const sPoint&);
	sPoint& operator/= (const sPoint&);
};

/////////////////////////////////////////////////
ostream& operator<< (ostream&, const sPoint&);
istream& operator>> (istream&, const sPoint&);

/////////////////////////////////////////////////
float CalcAngle (float, float);

/////////////////////////////////////////////////
/*struct sMatrix
{
	union
	{
		struct
		{
			float m_11, m_12, m_13, m_14;
			float m_21, m_22, m_23, m_34;
			float m_31, m_32, m_33, m_34;
			float m_41, m_42, m_43, m_44;
		};
		float m[4][4];
	};

	sMatrix();
	sMatrix(const float const*);
	sMatrix(const sMatrix&);
	sMatrix(float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44);

	float& operator() (DWORD, DWORD);
	float  operator() (DWORD, DWORD) const;

	sMatrix operator- () const;

	sMatrix operator* (const sMatrix&) const;
	sMatrix operator+ (const sMatrix&) const;
	sMatrix operator- (const sMatrix&) const;

	sMatrix operator* (float) const;
	sMatrix operator/ (float) const;

	const sMatrix& operator*= (const sMatrix&);
	const sMatrix& operator+= (const sMatrix&);
	const sMatrix& operator-= (const sMatrix&);

	const sMatrix& operator*= (float);
	const sMatrix& operator/= (float);

	friend sMatrix operator* (float, const sMatrix&);

	bool operator== (const sMatrix&) const;
	bool operator!= (const sMatrix&) const;

	const sMatrix& Identity();
	const sMatrix& Transpose();
	const sMatrix& Inverse();

	const sMatrix& Translation(float, float, float);
	const sMatrix& Translation(const sVector3d&);

	const sMatrix& Rotation(float, float, float);
	const sMatrix& Rotation(const sVector3d&);
	const sMatrix& RotationX(float);
	const sMatrix& RotationY(float);
	const sMatrix& RotationZ(float);

	const sMatrix& Scaling(float, float, float);
	const sMatrix& Scaling(const sVector3d&);
};*/

/////////////////////////////////////////////////
sVector3d operator* (float, const sVector3d&);
ostream& operator<< (ostream&, const sPoint&);
istream& operator>> (istream&, const sPoint&);
float CalcAngle (float, float);

inline float DegToRad(float deg)
{	return deg * PI / 180;	}

/////////////////////////////////////////////////
#endif // __UTIL_H__