#include <cmath>

#include "Util.h"

/////////////////////////////////////////////////
sVector2d::sVector2d ()
{
	m_x = m_y = 0.0f;
}

/////////////////////////////////////////////////
sVector2d::sVector2d (float x, float y)
{
	m_x = x;
	m_y = y;
}

/////////////////////////////////////////////////
sVector2d::sVector2d (const sVector2d& v)
{
	m_x = v.m_x;
	m_y = v.m_y;
}

/////////////////////////////////////////////////
void sVector2d::Set (float x, float y)
{
	m_x = x;
	m_y = y;
}

/////////////////////////////////////////////////
const sVector2d& sVector2d::operator= (const sVector2d& v)
{
	m_x = v.m_x;
	m_y = v.m_y;
	return *this;
}

/////////////////////////////////////////////////
bool sVector2d::operator== (const sVector2d& v)
{
	return ((m_x == v.m_x) && (m_y == v.m_y));
}

/////////////////////////////////////////////////
bool sVector2d::operator!= (const sVector2d& v)
{
	return ((m_x != v.m_x) || (m_y != v.m_y));
}

/////////////////////////////////////////////////
sVector2d sVector2d::operator+ (const sVector2d& v)
{
	return sVector2d(m_x + v.m_x, m_y + v.m_y);
}

/////////////////////////////////////////////////
sVector2d sVector2d::operator- (const sVector2d& v)
{
	return sVector2d(m_x - v.m_x, m_y - v.m_y);
}

/////////////////////////////////////////////////
const sVector2d& sVector2d::operator+= (const sVector2d& v)
{
	m_x += v.m_x;
	m_y += v.m_y;
	return *this;
}

/////////////////////////////////////////////////
const sVector2d& sVector2d::operator-= (const sVector2d& v)
{
	m_x -= v.m_x;
	m_y -= v.m_y;
	return *this;
}

/////////////////////////////////////////////////
sVector2d sVector2d::operator* (float f)
{
	return sVector2d(m_x * f, m_y * f);
}

/////////////////////////////////////////////////
sVector2d sVector2d::operator/ (float f)
{
	return sVector2d(m_x / f, m_y / f);
}

/////////////////////////////////////////////////
const sVector2d& sVector2d::operator*= (float f)
{
	m_x *= f;
	m_y *= f;
	return *this;
}

/////////////////////////////////////////////////
const sVector2d& sVector2d::operator/= (float f)
{
	m_x /= f;
	m_y /= f;
	return *this;
}

/////////////////////////////////////////////////
float sVector2d::Dot (const sVector2d& v)
{
	return (m_x * v.m_x + m_y * v.m_y);
}

/////////////////////////////////////////////////
float sVector2d::Length ()
{
	return sqrt(m_x * m_x + m_y * m_y);
}

/////////////////////////////////////////////////
float sVector2d::NormSquared ()
{
	return m_x * m_x + m_y * m_y;
}

/////////////////////////////////////////////////
sVector2d sVector2d::Normalize ()
{
	sVector2d result;
	float len = Length ();
	if (len > 0.001)
	{
		result.m_x = m_x / len;
		result.m_y = m_y / len;
	}
	return result;
}

/////////////////////////////////////////////////
sVector2d operator* (float f, const sVector2d& v)
{
	return sVector2d(v.m_x * f, v.m_y * f);
}

/////////////////////////////////////////////////
sVector2d operator/ (float f, const sVector2d& v)
{
	return sVector2d(v.m_x / f, v.m_y / f);
}

/////////////////////////////////////////////////
sVector3d::sVector3d ()
{
	m_x = m_y = m_z = 0.0f;
}

/////////////////////////////////////////////////
sVector3d::sVector3d (float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

/////////////////////////////////////////////////
sVector3d::sVector3d (const sVector3d& v)
{
	m_x = v.m_x;
	m_y = v.m_y;
	m_z = v.m_z;
}

/////////////////////////////////////////////////
void sVector3d::Set (float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

/////////////////////////////////////////////////
const sVector3d& sVector3d::operator= (const sVector3d& v)
{
	m_x = v.m_x;
	m_y = v.m_y;
	m_z = v.m_z;
	return *this;
}

/////////////////////////////////////////////////
bool sVector3d::operator== (const sVector3d& v)
{
	return ((m_x == v.m_x) && (m_y == v.m_y) && (m_z == v.m_z));
}

/////////////////////////////////////////////////
bool sVector3d::operator!= (const sVector3d& v)
{
	return ((m_x != v.m_x) || (m_y != v.m_y) || (m_z != v.m_z));
}

/////////////////////////////////////////////////
sVector3d sVector3d::operator+ (const sVector3d& v) const
{
	return sVector3d(m_x+v.m_x, m_y+v.m_y, m_z+v.m_z);
}

/////////////////////////////////////////////////
sVector3d sVector3d::operator- (const sVector3d& v) const
{
	return sVector3d(m_x-v.m_x, m_y-v.m_y, m_z-v.m_z);
}

/////////////////////////////////////////////////
const sVector3d& sVector3d::operator+= (const sVector3d& v)
{
	m_x += v.m_x;
	m_y += v.m_y;
	m_z += v.m_z;
	return *this;
}

/////////////////////////////////////////////////
const sVector3d& sVector3d::operator-= (const sVector3d& v)
{
	m_x -= v.m_x;
	m_y -= v.m_y;
	m_z -= v.m_z;
	return *this;
}

/////////////////////////////////////////////////
sVector3d sVector3d::operator* (float f)
{
	return sVector3d(m_x * f, m_y * f,  m_z * f);
}

/////////////////////////////////////////////////
sVector3d sVector3d::operator/ (float f)
{
	return sVector3d(m_x / f, m_y / f,  m_z / f);
}

/////////////////////////////////////////////////
const sVector3d& sVector3d::operator*= (float f)
{
	m_x *= f;
	m_y *= f;
	m_z *= f;
	return *this;
}

/////////////////////////////////////////////////
const sVector3d& sVector3d::operator/= (float f)
{
	m_x /= f;
	m_y /= f;
	m_z /= f;
	return *this;
}

/////////////////////////////////////////////////
sVector3d sVector3d::operator* (const sVector3d& v)
{
	return sVector3d(m_x * v.m_x, m_y * v.m_y,  m_z * v.m_z);
}

/////////////////////////////////////////////////
sVector3d sVector3d::operator/ (const sVector3d& v)
{
	return sVector3d(m_x / v.m_x, m_y / v.m_y,  m_z / v.m_z);
}


/////////////////////////////////////////////////
const sVector3d& sVector3d::operator*= (const sVector3d& v)
{
	m_x *= v.m_x;
	m_y *= v.m_y;
	m_z *= v.m_z;
	return *this;
}

/////////////////////////////////////////////////
const sVector3d& sVector3d::operator/= (const sVector3d& v)
{
	m_x /= v.m_x;
	m_y /= v.m_y;
	m_z /= v.m_z;
	return *this;
}

/////////////////////////////////////////////////
float sVector3d::Dot (const sVector3d& v)
{
	return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;
}

/////////////////////////////////////////////////
sVector3d sVector3d::Cross (const sVector3d& v)
{
	return sVector3d(m_y*v.m_z - m_z*v.m_y,
		m_z*v.m_x - m_x*v.m_z,
		m_x*v.m_y - m_y*v.m_x);
}

/////////////////////////////////////////////////
float sVector3d::Length ()
{
	return sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
}

/////////////////////////////////////////////////
float sVector3d::NormSquared ()
{
	return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
}

/////////////////////////////////////////////////
sVector3d sVector3d::Normalize ()
{
	sVector3d result;
	float len = Length ();
	if (len >= 0.001f)
	{
		result.m_x = m_x / len;
		result.m_y = m_y / len;
		result.m_z = m_z / len;
	}
	return result;
}

/////////////////////////////////////////////////
sPoint::sPoint ()  
{  m_x = m_y = 0;  }

/////////////////////////////////////////////////
sPoint::sPoint (int x, int y)  
{  m_x = x;  m_y = y;  }

/////////////////////////////////////////////////
void sPoint::Set (int x, int y)
{  m_x = x;  m_y = y;  }

/////////////////////////////////////////////////
float sPoint::Distance (const sPoint& pt)
{
	return sqrt ((float)
		((m_x - pt.m_x) * (m_x - pt.m_x) +
		(m_y - pt.m_y) * (m_y - pt.m_y)));
}

/////////////////////////////////////////////////
bool sPoint::operator== (const sPoint& pt)
{ 
	return ((m_x == pt.m_x) && (m_y == pt.m_y));
}

/////////////////////////////////////////////////
bool sPoint::operator!= (const sPoint& pt)
{
	return ((m_x != pt.m_x) || (m_y != pt.m_y));
}

/////////////////////////////////////////////////
sPoint sPoint::operator+ (const sPoint& pt)
{ 
	return sPoint (m_x + pt.m_x, m_y + pt.m_y);  
}

/////////////////////////////////////////////////
sPoint sPoint::operator- (const sPoint& pt)
{ 
	return sPoint (m_x - pt.m_x, m_y - pt.m_y);  
}

/////////////////////////////////////////////////
sPoint sPoint::operator* (const sPoint& pt)
{ 
	return sPoint (m_x * pt.m_x, m_y * pt.m_y);  
}

/////////////////////////////////////////////////
sPoint sPoint::operator/ (const sPoint& pt)
{ 
	return sPoint (m_x / pt.m_x, m_y / pt.m_y);  
}

/////////////////////////////////////////////////
sPoint sPoint::operator+ (int n)
{ 
	return sPoint (m_x + n, m_y + n);  
}

/////////////////////////////////////////////////
sPoint sPoint::operator- (int n)
{ 
	return sPoint (m_x - n, m_y - n);  
}

/////////////////////////////////////////////////
sPoint sPoint::operator* (int n)
{ 
	return sPoint (m_x * n, m_y * n);  
}

/////////////////////////////////////////////////
sPoint sPoint::operator/ (int n)
{ 
	return sPoint (m_x / n, m_y / n);  
}

/////////////////////////////////////////////////
sPoint& sPoint::operator+= (const sPoint& pt)
{ 
	m_x += pt.m_x;
	m_y += pt.m_y;

	return *this; 
}

/////////////////////////////////////////////////
sPoint& sPoint::operator-= (const sPoint& pt)
{ 
	m_x -= pt.m_x;
	m_y -= pt.m_y;

	return *this; 
}

/////////////////////////////////////////////////
sPoint& sPoint::operator*= (const sPoint& pt)
{ 
	m_x *= pt.m_x;
	m_y *= pt.m_y;

	return *this; 
}

/////////////////////////////////////////////////
sPoint& sPoint::operator/= (const sPoint& pt)
{ 
	m_x /= pt.m_x;
	m_y /= pt.m_y;

	return *this; 
}

/////////////////////////////////////////////////
sPoint& sPoint::operator+= (int n)
{ 
	m_x += n;
	m_y += n;

	return *this; 
}

/////////////////////////////////////////////////
sPoint& sPoint::operator-= (int n)
{ 
	m_x -= n;
	m_y -= n;

	return *this; 
}

/////////////////////////////////////////////////
sPoint& sPoint::operator*= (int n)
{ 
	m_x *= n;
	m_y *= n;

	return *this; 
}

/////////////////////////////////////////////////
sPoint& sPoint::operator/= (int n)
{ 
	m_x /= n;
	m_y /= n;

	return *this; 
}

/////////////////////////////////////////////////
/*sMatrix::sMatrix()
{
for(int row = 0; row < 4; ++row)
for(int col = 0; col < 4; ++row)
m[row][col] = 0;
}

/////////////////////////////////////////////////
sMatrix::sMatrix(const float const* f)
{
float* p = m;
for(int i = 0; i < 16; ++i)
m[i]i = f[i];
}

/////////////////////////////////////////////////
sMatrix::sMatrix(const sMatrix& mat)
{
for(int row = 0; row < 4; ++row)
for(int col = 0; col < 4; ++row)
m[row][col] = mat.m[row][col];
}

/////////////////////////////////////////////////
sMatrix::sMatrix(
float _11, float _12, float _13, float _14,
float _21, float _22, float _23, float _24,
float _31, float _32, float _33, float _34,
float _41, float _42, float _43, float _44)
{
m_11 = _11;  m_12 = _12;  m_13 = _13;  m_14 = _14;
m_21 = _21;  m_22 = _22;  m_23 = _23;  m_24 = _24;
m_31 = _31;  m_32 = _32;  m_33 = _33;  m_34 = _34;
m_41 = _41;  m_42 = _42;  m_43 = _43;  m_44 = _44;
}

/////////////////////////////////////////////////
float& sMatrix::operator() (DWORD row, DWORD col)
{
}

/////////////////////////////////////////////////
float  sMatrix::operator() (DWORD, DWORD) const
{
}

/////////////////////////////////////////////////
sMatrix sMatrix::operator- () const
{
}

/////////////////////////////////////////////////
sMatrix sMatrix::operator* (const sMatrix&) const
{
}

/////////////////////////////////////////////////
sMatrix sMatrix::operator+ (const sMatrix&) const
{
}

/////////////////////////////////////////////////
sMatrix sMatrix::operator- (const sMatrix&) const
{
}

/////////////////////////////////////////////////
sMatrix sMatrix::operator* (float) const
{
}

/////////////////////////////////////////////////
sMatrix sMatrix::operator/ (float) const
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::operator*= (const sMatrix&)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::operator+= (const sMatrix&)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::operator-= (const sMatrix&)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::operator*= (float)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::operator/= (float)
{
}

/////////////////////////////////////////////////
bool sMatrix::operator== (const sMatrix&) const
{
}

/////////////////////////////////////////////////
bool sMatrix::operator!= (const sMatrix&) const
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Identity()
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Transpose()
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Inverse()
{
for(i=0;i<r-1;i++)
{
for(j=i+1;j<r;j++)
{
l=lcm(a[i][i],a[j][i]);
if(l != 0 && (a[i][i] != 0 && a[j][i] != 0))
{
l=(a[i][i]*a[j][i])/l;
d1=l/a[i][i];
d2=l/a[j][i];
a[j][i]=0;
for(k=i+1;k<r;k++)
{
a[j][k]=(d2*a[j][k])-(d1*a[i][k]);
}
}
}
}
cout<<"\nThe given Matrix after Gauss Elimination is:\n";
for(i=0;i<r;i++)
{
for(j=0;j<c;j++)
{
cout<<a[i][j]<<"\t";
}
cout<<"\n";
}
}
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Translation(float, float, float)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Translation(const sVector3d&)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Rotation(float, float, float)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Rotation(const sVector3d&)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::RotationX(float)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::RotationY(float)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::RotationZ(float)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Scaling(float, float, float)
{
}

/////////////////////////////////////////////////
const sMatrix& sMatrix::Scaling(const sVector3d&)
{
}

/////////////////////////////////////////////////*/
sVector3d operator* (float f, const sVector3d& v)
{
	return sVector3d(v.m_x * f, v.m_y * f,  v.m_z * f);
}

/////////////////////////////////////////////////
ostream& operator<< (ostream& out, const sPoint& pt)
{
	out << '(' << pt.m_x << ", " << pt.m_y << ')';

	return out;
}

/////////////////////////////////////////////////
istream& operator>> (istream& in, sPoint& pt)
{
	in >> pt.m_x >> pt.m_y;

	return in;
}

/////////////////////////////////////////////////
float CalcAngle (float x, float y)
{
	float speed = sqrt((x * x) + (y * y)),
		angle = 0.0f;

	angle = (float)acos(y/speed);
	if ((float)asin(x/speed) <= 0.0f)
		angle = (6.28319f - angle);

	return angle;
}

