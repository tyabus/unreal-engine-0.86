/*=============================================================================
	UnMath.h: Unreal math routines

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNMATH // Prevent header from being included multiple times
#define _INC_UNMATH

/*-----------------------------------------------------------------------------
	Declarations.
-----------------------------------------------------------------------------*/

class  FVector;
class  FPlane;
class  FCoords;
class  FRotation;
class  FScale;
class  FGlobalMath;

/*-----------------------------------------------------------------------------
	Constants.
-----------------------------------------------------------------------------*/

#define PI 					(3.1415926535897932)
#define SQRT2				(1.4142135623730950)
#define SQRT3				(1.7320508075688772)
#define LOGE2				(0.6931471805599453)
#define SMALL_NUMBER		(1.e-8)
#define KINDA_SMALL_NUMBER	(1.e-4)

/*-----------------------------------------------------------------------------
	Templates.
-----------------------------------------------------------------------------*/

// Fixed point conversion.
inline	int Fix		(int A)			{return A<<16;};
inline	int Fix		(FLOAT A)		{return A*65536.0;};
inline	int Unfix	(int A)			{return A>>16;};

// Standard templates.
template <class T> inline T Abs    ( const T A            ) {return (A>=(T)0) ? A : -A;}
template <class T> inline T Sgn    ( const T A            ) {return (A>0) ? 1 : ((A<0) ? -1 : 0);}
template <class T> inline T	Max    ( const T A, const T B ) {return (A>=B) ? A : B;}
template <class T> inline T	Min    ( const T A, const T B ) {return (A<=B) ? A : B;}
template <class T> inline T	Square ( const T A            ) {return A*A;}
template <class T> inline T Clamp  ( const T X, const T Min, const T Max ) {return X<Min ? Min : X<Max ? X : Max;}
template <class T> inline T Align  ( const T Ptr, INT Alignment ) {return (T)(((DWORD)Ptr + Alignment - 1) & ~(Alignment-1));}

/*-----------------------------------------------------------------------------
	Global functions.
-----------------------------------------------------------------------------*/

//
// Snap a value to the nearest grid multiple.
//
inline FLOAT FSnap( FLOAT Location, FLOAT Grid )
{
	if( Grid==0.0 )	return Location;
	else			return floor((Location + 0.5*Grid)/Grid)*Grid;
}

//
// Internal sheer adjusting function so it snaps nicely at 0 and 45 degrees.
//
inline FLOAT FSheerSnap (FLOAT Sheer)
{
	if		(Sheer < -0.65)	return Sheer + 0.15;
	else if (Sheer > +0.65)	return Sheer - 0.15;
	else if (Sheer < -0.55)	return -0.50;
	else if (Sheer > +0.55)	return 0.50;
	else if (Sheer < -0.05)	return Sheer + 0.05;
	else if (Sheer > +0.05)	return Sheer - 0.05;
	else					return 0.0;
}

//
// Find the closest power of 2 that is >= N.
//
inline DWORD FNextPowerOfTwo( DWORD N )
{
	if (N<=0L		) return 0L;
	if (N<=1L		) return 1L;
	if (N<=2L		) return 2L;
	if (N<=4L		) return 4L;
	if (N<=8L		) return 8L;
	if (N<=16L	    ) return 16L;
	if (N<=32L	    ) return 32L;
	if (N<=64L 	    ) return 64L;
	if (N<=128L     ) return 128L;
	if (N<=256L     ) return 256L;
	if (N<=512L     ) return 512L;
	if (N<=1024L    ) return 1024L;
	if (N<=2048L    ) return 2048L;
	if (N<=4096L    ) return 4096L;
	if (N<=8192L    ) return 8192L;
	if (N<=16384L   ) return 16384L;
	if (N<=32768L   ) return 32768L;
	if (N<=65536L   ) return 65536L;
	else			  return 0;
}

//
// Find the largest number whose base-two log is <= N.
//
inline DWORD FLogTwo( DWORD N )
{
	if (N<=0L		) return 0;
	if (N<=1L		) return 0;
	if (N<=2L		) return 1;
	if (N<=4L		) return 2;
	if (N<=8L		) return 3;
	if (N<=16L	    ) return 4;
	if (N<=32L	    ) return 5;
	if (N<=64L 	    ) return 6;
	if (N<=128L     ) return 7;
	if (N<=256L     ) return 8;
	if (N<=512L     ) return 9;
	if (N<=1024L    ) return 10;
	if (N<=2048L    ) return 11;
	if (N<=4096L    ) return 12;
	if (N<=8192L    ) return 13;
	if (N<=16384L   ) return 14;
	if (N<=32768L   ) return 15;
	if (N<=65536L   ) return 16;
	else			  return 0;
}

//
// Add to a word angle, constraining it within a min (not to cross)
// and a max (not to cross).  Accounts for funkyness of word angles.
// Assumes that angle is initially in the desired range.
//
inline WORD FAddAngleConfined( INT Angle, INT Delta, INT MinThresh, INT MaxThresh )
{
	if( Delta < 0 )
	{
		if ( Delta<=-0x10000L || Delta<=-(INT)((WORD)(Angle-MinThresh)))
			return MinThresh;
	}
	else if( Delta > 0 )
	{
		if( Delta>=0x10000L || Delta>=(INT)((WORD)(MaxThresh-Angle)))
			return MaxThresh;
	}
	return (WORD)(Angle+Delta);
}

//
// Eliminate all fractional precision from an angle.
//
INT ReduceAngle( INT Angle );

/*-----------------------------------------------------------------------------
	FVector.
-----------------------------------------------------------------------------*/

// Information associated with a floating point vector, describing its
// status as a point in a rendering context.
enum EVectorFlags
{
	FVF_OutXMin		= 0x04,	// Outcode rejection, off left hand side of screen.
	FVF_OutXMax		= 0x08,	// Outcode rejection, off right hand side of screen.
	FVF_OutYMin		= 0x10,	// Outcode rejection, off top of screen.
	FVF_OutYMax		= 0x20,	// Outcode rejection, off bottom of screen.
	FVF_OutReject   = (FVF_OutXMin | FVF_OutXMax | FVF_OutYMin | FVF_OutYMax), // Outcode rejectable.
	FVF_OutSkip		= (FVF_OutXMin | FVF_OutXMax | FVF_OutYMin | FVF_OutYMax), // Outcode clippable.
};

//
// Floating point vector.
//
class UNENGINE_API FVector 
{
public:
	union
	{
		struct {FLOAT X,Y,Z;};			// Coordinates.
		struct {FLOAT R,G,B;};			// Color components.
		struct {FLOAT V[3];};			// Indexed components.
	};

	// Constructors.
	FVector() {}
	FVector( FLOAT InX, FLOAT InY, FLOAT InZ )
	:	X(InX), Y(InY), Z(InZ) {};

	// Binary math operators.
	FVector operator^( const FVector &V ) const
	{
		return FVector
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}
	FLOAT operator|( const FVector &V ) const
	{
		return X*V.X + Y*V.Y + Z*V.Z;
	}
	friend FVector operator*( FLOAT Scale,const FVector &V )
	{
		return FVector( V.X * Scale, V.Y * Scale, V.Z * Scale );
	}
	FVector operator+( const FVector &V ) const
	{
		return FVector( X + V.X, Y + V.Y, Z + V.Z );
	}
	FVector operator-( const FVector &V ) const
	{
		return FVector( X - V.X, Y - V.Y, Z - V.Z );
	}
	FVector operator*( FLOAT Scale ) const
	{
		return FVector( X * Scale, Y * Scale, Z * Scale );
	}
	FVector operator/( FLOAT Scale ) const
	{
		FLOAT RScale = 1.0/Scale;
		return FVector( X * RScale, Y * RScale, Z * RScale );
	}
	FVector operator*( const FVector &V ) const
	{
		return FVector( X * V.X, Y * V.Y, Z * V.Z );
	}

	// Binary comparison operators.
	int operator==( const FVector &V ) const
	{
		return X==V.X && Y==V.Y && Z==V.Z;
	}
	int operator!=( const FVector &V ) const
	{
		return X!=V.X || Y!=V.Y || Z!=V.Z;
	}

	// Unary operators.
	FVector operator-() const
	{
		return FVector( -X, -Y, -Z );
	}
	// Assignment operators.
	FVector operator+=( const FVector &V )
	{
		X += V.X; Y += V.Y; Z += V.Z;
		return *this;
	}
	FVector operator-=( const FVector &V )
	{
		X -= V.X; Y -= V.Y; Z -= V.Z;
		return *this;
	}
	FVector operator*=( FLOAT Scale )
	{
		X *= Scale; Y *= Scale; Z *= Scale;
		return *this;
	}
	FVector operator/=( FLOAT V )
	{
		FLOAT RV = 1.0/V;
		X *= RV; Y *= RV; Z *= RV;
		return *this;
	}
	FVector operator*=( FVector &V )
	{
		X *= V.X; Y *= V.Y; Z *= V.Z;
		return *this;
	}

	// Simple functions.
	FLOAT Size() const
	{
		return sqrt(X*X+Y*Y+Z*Z);
	}
	FLOAT SizeSquared() const
	{
		return X*X + Y*Y + Z*Z;
	}
	FLOAT Size2D() const 
	{
		return sqrt( X*X + Y*Y );
	}
	FLOAT SizeSquared2D() const 
	{
		return sqrt( X*X + Y*Y );
	}
	int IsNearlyZero() const
	{
		return Abs(X)<KINDA_SMALL_NUMBER && Abs(Y)<KINDA_SMALL_NUMBER && Abs(Z)<KINDA_SMALL_NUMBER;
	}
	int IsZero() const
	{
		return X==0.0 && Y==0.0 && Z==0.0;
	}
	int Normalize()
	{
		FLOAT SquareSum = X*X+Y*Y+Z*Z;
		if( SquareSum >= SMALL_NUMBER )
		{
			FLOAT Scale = 1.0/sqrt(SquareSum);
			X *= Scale; Y *= Scale; Z *= Scale;
			return 1;
		}
		else return 0;
	}
	FVector Normal() const
	{
		FLOAT SquareSum = X*X + Y*Y + Z*Z;
		if( SquareSum >= SMALL_NUMBER )
		{
			FLOAT Scale = 1.0/sqrt (SquareSum);
			return FVector( X*Scale, Y*Scale, Z*Scale );
		}
		else return FVector( 0.0, 0.0, 0.0 );
	}
	FVector GridSnap( const FVector &Grid )
	{
		return FVector( FSnap(X, Grid.X),FSnap(Y, Grid.Y),FSnap(Z, Grid.Z) );
	}
	FVector BoundToCube( FLOAT Radius )
	{
		return FVector
		(
			Clamp(X,-Radius,Radius),
			Clamp(Y,-Radius,Radius),
			Clamp(Z,-Radius,Radius)
		);
	}
	void AddBounded( const FVector &V, FLOAT Radius=MAXSWORD )
	{
		*this = (*this + V).BoundToCube(Radius);
	}

	// Return a boolean that is based on the vector's direction.
	// When      V==zero vector Booleanize(0)=1.
	// Otherwise Booleanize(V) <-> !Booleanize(!B).
	BOOL Booleanize()
	{
		return
			X >  0.0 ? 1 :
			X <  0.0 ? 0 :
			Y >  0.0 ? 1 :
			Y <  0.0 ? 0 :
			Z >= 0.0 ? 1 : 0;
	}
	void GetHSV( BYTE H, BYTE S, BYTE V, INT ColorBytes );

	// Transformation.
	FVector TransformVectorBy					(const FCoords &Coords) const;
	FVector TransformPointBy					(const FCoords &Coords) const;
	FVector MirrorBy							(const FVector &MirrorNormal) const;

	// Complicated functions.
	FRotation		Rotation					();
	void			FindBestAxisVectors			(FVector &Axis1, FVector &Axis2);

	// Friends.
	friend FLOAT   FDist				 (const FVector &V1, const FVector &V2);
	friend FLOAT   FDistSquared			 (const FVector &V1, const FVector &V2);
	friend int     FPointsAreSame        (const FVector &P, const FVector &Q);
	friend int     FPointsAreNear        (const FVector &Point1,const FVector &Point2, FLOAT Dist);
	friend FLOAT   FPointPlaneDist       (const FVector &Point,const FVector &PlaneBase,const FVector &PlaneNormal);
	friend FVector FLinePlaneIntersection(const FVector &Point1,const FVector &Point2,const FVector &PlaneOrigin,const FVector &PlaneNormal);
	friend FVector FLinePlaneIntersection(const FVector &Point1,const FVector &Point2,const FPlane &Plane);
	friend int     FParallel             (const FVector &Normal1, const FVector &Normal2);
	friend int     FCoplanar             (const FVector &Base1, const FVector &Normal1, const FVector &Base2, const FVector &Normal2);

	// Serializer.
	friend FArchive& operator<<( FArchive &Ar, FVector &V )
	{
		return Ar << V.X << V.Y << V.Z;
	}
};

/*-----------------------------------------------------------------------------
	FPlane.
-----------------------------------------------------------------------------*/

class UNENGINE_API FPlane : public FVector
{
public:
	// Variables.
	FLOAT W;

	// Constructors.
	FPlane() {}
	FPlane( const FVector &V )
	:	FVector(V)
	,	W(0)
	{}
	FPlane( FLOAT InX, FLOAT InY, FLOAT InZ, FLOAT InW )
	:	FVector(InX,InY,InZ)
	,	W(InW)
	{}
	FPlane( FVector InNormal, FLOAT InW )
	:	FVector(InNormal), W(InW)
	{}
	FPlane( FVector InBase, const FVector &InNormal )
	:	FVector(InNormal)
	,	W(InBase | InNormal)
	{}
	FPlane( FVector A, FVector B, FVector C )
	:	FVector( ((B-A)^(C-A)).Normal() )
	,	W( A | ((B-A)^(C-A)).Normal() )
	{}

	// Functions.
	FLOAT PlaneDot( const FVector &P ) const
	{
		return X*P.X + Y*P.Y + Z*P.Z - W;
	}
	FPlane Flip() const
	{
		return FPlane(-X,-Y,-Z,-W);
	}
	FPlane TransformPlaneByOrtho( const FCoords &Coords ) const;

	// Serializer.
	friend FArchive& operator<<( FArchive &Ar, FPlane &P )
	{
		return Ar << (FVector&)P << P.W;
	}
};

/*-----------------------------------------------------------------------------
	FScale.
-----------------------------------------------------------------------------*/

// An axis along which sheering is performed.
enum ESheerAxis
{
	SHEER_None = 0,
	SHEER_XY   = 1,
	SHEER_XZ   = 2,
	SHEER_YX   = 3,
	SHEER_YZ   = 4,
	SHEER_ZX   = 5,
	SHEER_ZY   = 6,
};

//
// Scaling and sheering info associated with a brush.  This is 
// easily-manipulated information which is built into a transformation
// matrix later.
//
class UNENGINE_API FScale 
{
public:
	// Variables.
	FVector		Scale;
	FLOAT		SheerRate;
	INT			SheerAxis; // From ESheerAxis

	// Serializer.
	friend FArchive& operator<< (FArchive &Ar, FScale &S )
		{return Ar << S.Scale << S.SheerRate << S.SheerAxis;}

	// Constructors.
	FScale() {}
	FScale( const FVector &InScale, FLOAT InSheerRate, ESheerAxis InSheerAxis )
	:	Scale(InScale), SheerRate(InSheerRate), SheerAxis(InSheerAxis) {}

	// Operators.
	int operator==( const FScale &S ) const
	{
		return Scale==S.Scale && SheerRate==S.SheerRate && SheerAxis==S.SheerAxis;
	}

	// Functions.
	FLOAT  Orientation()
	{
		return Sgn(Scale.X * Scale.Y * Scale.Z);
	}
};

/*-----------------------------------------------------------------------------
	FCoords.
-----------------------------------------------------------------------------*/

//
// A coordinate system matrix.
//
class UNENGINE_API FCoords
{
public:
	FVector	Origin;
	FVector	XAxis;
	FVector YAxis;
	FVector ZAxis;

	// Constructors.
	FCoords() {}
	FCoords( const FVector &InOrigin )
	:	Origin(InOrigin), XAxis(1,0,0), YAxis(0,1,0), ZAxis(0,0,1) {}
	FCoords( const FVector &InOrigin, const FVector &InX, const FVector &InY, const FVector &InZ )
	:	Origin(InOrigin), XAxis(InX), YAxis(InY), ZAxis(InZ) {}

	// Operators.
	FCoords	 Transpose	() const;
	FCoords  Inverse    () const;
	FCoords& operator*=	(const FCoords   &TransformCoords);
	FCoords	 operator*	(const FCoords   &TransformCoords) const;
	FCoords& operator*=	(const FVector   &Point);
	FCoords  operator*	(const FVector   &Point) const;
	FCoords& operator*=	(const FRotation &Rot);
	FCoords  operator*	(const FRotation &Rot) const;
	FCoords& operator*=	(const FScale    &Scale);
	FCoords  operator*	(const FScale    &Scale) const;
	FCoords& operator/=	(const FVector   &Point);
	FCoords  operator/	(const FVector   &Point) const;
	FCoords& operator/=	(const FRotation &Rot);
	FCoords  operator/	(const FRotation &Rot) const;
	FCoords& operator/=	(const FScale    &Scale);
	FCoords  operator/	(const FScale    &Scale) const;
};

/*-----------------------------------------------------------------------------
	FModelCoords.
-----------------------------------------------------------------------------*/

//
// A model coordinate system, describing both the covariant and contravariant
// transformation matrices to transform points and normals by.
//
class UNENGINE_API FModelCoords
{
public:
	// Variables.
	FCoords PointXform;		// Coordinates to transform points by  (covariant).
	FCoords VectorXform;	// Coordinates to transform normals by (contravariant).

	// Constructors.
	FModelCoords()
	{}
	FModelCoords( const FCoords& InCovariant, const FCoords& InContravariant )
	:	PointXform(InCovariant), VectorXform(InContravariant)
	{}

	// Functions.
	FModelCoords Inverse()
	{
		return FModelCoords( VectorXform.Transpose(), PointXform.Transpose() );
	}
};

/*-----------------------------------------------------------------------------
	FRotation.
-----------------------------------------------------------------------------*/

//
// Rotation.
//
class UNENGINE_API FRotation
{
public:
	// Variables.
	INT Pitch; // Looking up and down (0=Straight Ahead, +Up, -Down).
	INT Yaw;   // Rotating around (running in circles), 0=East, +North, -South.
	INT Roll;  // Rotation about axis of screen, 0=Straight, +Clockwise, -CCW.

	// Serializer.
	friend FArchive& operator<< (FArchive &Ar, FRotation &R )
	{
		return Ar << R.Pitch << R.Yaw << R.Roll;
	}

	// Constructors.
	FRotation() {}
	FRotation( INT InPitch, INT InYaw, INT InRoll )
	:	Pitch(InPitch), Yaw(InYaw), Roll(InRoll) {}

	// Binary arithmetic operators.
	FRotation operator+( const FRotation &R ) const
	{
		return FRotation( Pitch+R.Pitch, Yaw+R.Yaw, Roll+R.Roll );
	}
	FRotation operator-( const FRotation &R ) const
	{
		return FRotation( Pitch-R.Pitch, Yaw-R.Yaw, Roll-R.Roll );
	}
	FRotation operator*( FLOAT Scale ) const
	{
		return FRotation( Pitch*Scale, Yaw*Scale, Roll*Scale );
	}
	friend FRotation operator*( FLOAT Scale, const FRotation &R )
	{
		return FRotation( R.Pitch*Scale, R.Yaw*Scale, R.Roll*Scale );
	}
	FRotation operator*= (FLOAT Scale)
	{
		Pitch *= Scale; Yaw *= Scale; Roll *= Scale;
		return *this;
	}
	// Binary comparison operators.
	int operator==( const FRotation &R ) const
	{
		return Pitch==R.Pitch && Yaw==R.Yaw && Roll==R.Roll;
	}
	int operator!=( const FRotation &V ) const
	{
		return Pitch!=V.Pitch || Yaw!=V.Yaw || Roll!=V.Roll;
	}
	// Assignment operators.
	FRotation operator+=( const FRotation &R )
	{
		Pitch += R.Pitch; Yaw += R.Yaw; Roll += R.Roll;
		return *this;
	}
	FRotation operator-=( const FRotation &R )
	{
		Pitch -= R.Pitch; Yaw -= R.Yaw; Roll -= R.Roll;
		return *this;
	}
	// Functions.
	FRotation Reduce() const
	{
		return FRotation( ReduceAngle(Pitch), ReduceAngle(Yaw), ReduceAngle(Roll) );
	}
	int IsZero() const
	{
		return ((Pitch&65535)==0) && ((Yaw&65535)==0) && ((Roll&65535)==0);
	}
	FRotation Add( INT DeltaPitch, INT DeltaYaw, INT DeltaRoll )
	{
		Yaw   += DeltaYaw;
		Pitch += DeltaPitch;
		Roll  += DeltaRoll;
		return *this;
	}
	FRotation AddBounded( INT DeltaPitch, INT DeltaYaw, INT DeltaRoll )
	{
		Yaw  += DeltaYaw;
		Pitch = FAddAngleConfined(Pitch,DeltaPitch,192*0x100,64*0x100);
		Roll  = FAddAngleConfined(Roll, DeltaRoll, 192*0x100,64*0x100);
		return *this;
	}
	FRotation GridSnap( const FRotation &RotGrid )
	{
		return FRotation
		(
			FSnap(Pitch,RotGrid.Pitch),
			FSnap(Yaw,  RotGrid.Yaw),
			FSnap(Roll, RotGrid.Roll)
		);
	}
	FVector Vector();
};

/*-----------------------------------------------------------------------------
	Bounds.
-----------------------------------------------------------------------------*/

//
// A rectangular minimum bounding volume.
//
class UNENGINE_API FBoundingBox
{
public:
	// Variables.
	FVector Min;
	FVector Max;
	BYTE IsValid;

	// Constructors.
	FBoundingBox()
	{}
	FBoundingBox( const FVector& InMin, const FVector& InMax )
	:	Min		(InMin)
	,	Max		(InMax)
	,	IsValid	(1)
	{}
	FBoundingBox( int NumPts, FVector *Points=NULL )
	:	Min		(0,0,0)
	,	Max		(0,0,0)
	,	IsValid	(0)
	{
		for( int i=0; i<NumPts; i++ )
			*this += Points[i];
	}

	// Accessors.
	FVector& GetExtrema( int i )
	{
		return (&Min)[i];
	}
	const FVector& GetExtrema( int i ) const
	{
		return (&Min)[i];
	}

	// Functions.
	FBoundingBox& operator+=( const FVector &Other )
	{
		if( IsValid )
		{
			Min.X = ::Min( Min.X, Other.X );
			Min.Y = ::Min( Min.Y, Other.Y );
			Min.Z = ::Min( Min.Z, Other.Z );

			Max.X = ::Max( Max.X, Other.X );
			Max.Y = ::Max( Max.Y, Other.Y );
			Max.Z = ::Max( Max.Z, Other.Z );
		}
		else
		{
			Min = Max = Other;
			IsValid = 1;
		}
		return *this;
	}
	FBoundingBox operator+( const FVector &Other ) const
	{
		return FBoundingBox(*this) += Other;
	}
	FBoundingBox& operator+=( const FBoundingBox &Other )
	{
		if( IsValid && Other.IsValid )
		{
			Min.X = ::Min( Min.X, Other.Min.X );
			Min.Y = ::Min( Min.Y, Other.Min.Y );
			Min.Z = ::Min( Min.Z, Other.Min.Z );

			Max.X = ::Max( Max.X, Other.Max.X );
			Max.Y = ::Max( Max.Y, Other.Max.Y );
			Max.Z = ::Max( Max.Z, Other.Max.Z );
		}
		else *this = Other;
		return *this;
	}
	FBoundingBox operator+( const FBoundingBox &Other ) const
	{
		return FBoundingBox(*this) += Other;
	}
	FBoundingBox TransformBy( const FCoords& Coords ) const
	{
		FBoundingBox NewBox(0);
		for( int i=0; i<2; i++ )
			for( int j=0; j<2; j++ )
				for( int k=0; k<2; k++ )
					NewBox += FVector( GetExtrema(i).X, GetExtrema(j).Y, GetExtrema(k).Z ).TransformPointBy( Coords );
		return NewBox;
	}
	FBoundingBox ExpandBy( FLOAT W ) const
	{
		return FBoundingBox( Min - FVector(W,W,W), Max + FVector(W,W,W) );
	}

	// Serializer.
	friend FArchive& operator<<( FArchive &Ar, FBoundingBox &Bound )
	{
		return Ar << Bound.Min << Bound.Max << Bound.IsValid;
	}
};

//
// A rectangular and spherical minimum bounding volume.
//
class UNENGINE_API FBoundingVolume : public FBoundingBox
{
public:
	// Variables.
	FPlane Sphere; // X,Y,Z = Center, W = radius.

	// Constructors.
	FBoundingVolume()
	{}
	FBoundingVolume( const FVector& InMin, const FVector& InMax, const FPlane& InSphere )
	:	FBoundingBox	(InMin,InMax)
	,	Sphere			(InSphere)
	{}
	FBoundingVolume( int NumPts, FVector *Points=NULL )
	:	FBoundingBox( NumPts, Points )
	,	Sphere      ( 0, 0, 0, 0     )
	{
		if( IsValid )
		{
			(FVector&)Sphere = 0.5*(Min+Max);
			FLOAT RSquared = 0.0;
			for( int i=0; i<NumPts; i++ )
				RSquared = ::Max( RSquared, FDistSquared( Points[i], Sphere ) );
			Sphere.W = sqrt(RSquared) * 1.01;
		}
	}
	
	// Functions.
	FBoundingVolume ExpandBy( FLOAT W ) const
	{
		return FBoundingVolume( Min - FVector(W,W,W), Max + FVector(W,W,W), FPlane(Sphere.X,Sphere.Y,Sphere.Z,Sphere.W+W) );
	}

	// Serializer.
	friend FArchive& operator<<( FArchive &Ar, FBoundingVolume &Bound )
	{
		return Ar << (FBoundingBox&)Bound << Bound.Sphere;
	}
};

/*-----------------------------------------------------------------------------
	FGlobalMath.
-----------------------------------------------------------------------------*/

//
// Global mathematics info.
//
class UNENGINE_API FGlobalMath
{
public:
	// Constants.
	enum {ANGLE_SHIFT 	= 2};		// Bits to right-shift to get lookup value.
	enum {ANGLE_BITS	= 14};		// Number of valid bits in angles.
	enum {NUM_ANGLES 	= 16384}; 	// Number of angles that are in lookup table.
	enum {NUM_SQRTS		= 16384};	// Number of square roots in lookup table.
	enum {ANGLE_MASK    =  (((1<<ANGLE_BITS)-1)<<(16-ANGLE_BITS))};

	// Class constants.
	const FVector  	WorldMin;
	const FVector  	WorldMax;
	const FCoords  	UnitCoords;
	const FScale   	UnitScale;
	const FCoords	CameraViewCoords;
	const FRotation	View0,View1,View2,View3,View4,View5;
	const FRotation	*Views[6];

	// Basic math functions.
	FLOAT Sqrt( int i )
	{
		return SqrtFLOAT[i]; 
	}
	FLOAT SinTab( int i )
	{
		return TrigFLOAT[((i>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	FLOAT CosTab( int i )
	{
		return TrigFLOAT[(((i+16384)>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	FLOAT SinFloat( FLOAT F )
	{
		return SinTab((F*65536)/(2.0*PI));
	}
	FLOAT CosFloat( FLOAT F )
	{
		return CosTab((F*65536)/(2.0*PI));
	}

	// Constructor.
	FGlobalMath();

private:
	// Tables.
	FLOAT  TrigFLOAT		[NUM_ANGLES];
	FLOAT  SqrtFLOAT		[NUM_SQRTS];
	FLOAT  LightSqrtFLOAT	[NUM_SQRTS];
};

inline INT ReduceAngle( INT Angle )
{
	return Angle & FGlobalMath::ANGLE_MASK;
};

/*-----------------------------------------------------------------------------
	Floating point constants.
-----------------------------------------------------------------------------*/

//
// Lengths of normalized vectors (These are half their maximum values
// to assure that dot products with normalized vectors don't overflow).
//
#define FLOAT_NORMAL_THRESH				(0.0001)

//
// Magic numbers for numerical precision.
//
#define THRESH_POINT_ON_PLANE			(0.10)		/* Thickness of plane for front/back/inside test */
#define THRESH_POINT_ON_SIDE			(0.20)		/* Thickness of polygon side's side-plane for point-inside/outside/on side test */
#define THRESH_POINTS_ARE_SAME			(0.002)		/* Two points are same if within this distance */
#define THRESH_POINTS_ARE_NEAR			(0.015)		/* Two points are near if within this distance and can be combined if imprecise math is ok */
#define THRESH_NORMALS_ARE_SAME			(0.00002)	/* Two normal points are same if within this distance */
													/* Making this too large results in incorrect CSG classification and disaster */
#define THRESH_VECTORS_ARE_NEAR			(0.0004)	/* Two vectors are near if within this distance and can be combined if imprecise math is ok */
													/* Making this too large results in lighting problems due to inaccurate texture coordinates */
#define THRESH_SPLIT_POLY_WITH_PLANE	(0.25)		/* A plane splits a polygon in half */
#define THRESH_SPLIT_POLY_PRECISELY		(0.01)		/* A plane exactly splits a polygon */
#define THRESH_ZERO_NORM_SQUARED		(0.01*0.01)	/* Size of a unit normal that is considered "zero", squared */
#define THRESH_VECTORS_ARE_PARALLEL		(0.02)		/* Vectors are parallel if dot product varies less than this */

/*-----------------------------------------------------------------------------
	FVector transformation.
-----------------------------------------------------------------------------*/

//
// Transform a directional vector by a coordinate system.
// Ignore's the coordinate system's origin.
//
inline FVector FVector::TransformVectorBy( const FCoords &Coords ) const
{
	return FVector(	*this | Coords.XAxis, *this | Coords.YAxis, *this | Coords.ZAxis );
}

//
// Transform a point by a coordinate system, moving
// it by the coordinate system's origin if nonzero.
//
inline FVector FVector::TransformPointBy( const FCoords &Coords ) const
{
	FVector Temp = *this - Coords.Origin;
	return FVector(	Temp | Coords.XAxis, Temp | Coords.YAxis, Temp | Coords.ZAxis );
}

//
// Mirror a vector about a normal vector.
//
inline FVector FVector::MirrorBy( const FVector &MirrorNormal ) const
{
	return *this - MirrorNormal * (2.0 * (*this | MirrorNormal));
}

/*-----------------------------------------------------------------------------
	FVector friends.
-----------------------------------------------------------------------------*/

//
// Compare two points and see if they're the same, using a threshold.
// Returns 1=yes, 0=no.  Uses fast distance approximation.
//
inline int FPointsAreSame( const FVector &P, const FVector &Q )
{
	FLOAT Temp;
	Temp=P.X-Q.X;
	if( (Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME) )
	{
		Temp=P.Y-Q.Y;
		if( (Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME) )
		{
			Temp=P.Z-Q.Z;
			if( (Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME) )
			{
				return 1;
			}
		}
	}
	return 0;
}

//
// Compare two points and see if they're the same, using a threshold.
// Returns 1=yes, 0=no.  Uses fast distance approximation.
//
inline int FPointsAreNear( const FVector &Point1, const FVector &Point2, FLOAT Dist )
{
	FLOAT Temp;
	Temp=(Point1.X - Point2.X); if (Abs(Temp)>=Dist) return 0;
	Temp=(Point1.Y - Point2.Y); if (Abs(Temp)>=Dist) return 0;
	Temp=(Point1.Z - Point2.Z); if (Abs(Temp)>=Dist) return 0;
	return 1;
}

//
// Calculate the signed distance (in the direction of the normal) between
// a point and a plane.
//
inline FLOAT FPointPlaneDist
(
	const FVector &Point,
	const FVector &PlaneBase,
	const FVector &PlaneNormal
)
{
	return (Point - PlaneBase) | PlaneNormal;
}

//
// Euclidean distance between two points.
//
inline FLOAT FDist( const FVector &V1, const FVector &V2 )
{
	return sqrt( Square(V2.X-V1.X) + Square(V2.Y-V1.Y) + Square(V2.Z-V1.Z) );
}

//
// Squared distance between two points.
//
inline FLOAT FDistSquared( const FVector &V1, const FVector &V2 )
{
	return Square(V2.X-V1.X) + Square(V2.Y-V1.Y) + Square(V2.Z-V1.Z);
}

//
// See if two normal vectors (or plane normals) are nearly parallel.
//
inline int FParallel( const FVector &Normal1, const FVector &Normal2 )
{
	FLOAT NormalDot = Normal1 | Normal2;
	return (Abs (NormalDot - 1.0) <= THRESH_VECTORS_ARE_PARALLEL);
}

//
// See if two planes are coplanar.
//
inline int FCoplanar( const FVector &Base1, const FVector &Normal1, const FVector &Base2, const FVector &Normal2 )
{
	if      (!FParallel(Normal1,Normal2)) return 0;
	else if (FPointPlaneDist (Base2,Base1,Normal1) > THRESH_POINT_ON_PLANE) return 0;
	else    return 1;
}

/*-----------------------------------------------------------------------------
	FPlane implementation.
-----------------------------------------------------------------------------*/

//
// Transform a point by a coordinate system, moving
// it by the coordinate system's origin if nonzero.
//
inline FPlane FPlane::TransformPlaneByOrtho( const FCoords &Coords ) const
{
	FVector Normal( *this | Coords.XAxis, *this | Coords.YAxis, *this | Coords.ZAxis );
	return FPlane( Normal, W - (Coords.Origin.TransformVectorBy(Coords) | Normal) );
}

/*-----------------------------------------------------------------------------
	FCoords implementation.
-----------------------------------------------------------------------------*/

//
// Return this coordinate system's transpose.
// If the coordinate system is orthogonal, this is equivalant to its inverse.
//
inline FCoords FCoords::Transpose() const
{
	return FCoords
	(
		-Origin.TransformVectorBy(*this),
		FVector( XAxis.X, YAxis.X, ZAxis.X ),
		FVector( XAxis.Y, YAxis.Y, ZAxis.Y ),
		FVector( XAxis.Z, YAxis.Z, ZAxis.Z )
	);
}

//
// Transform this coordinate system by another coordinate system.
//
inline FCoords& FCoords::operator*=( const FCoords &TransformCoords )
{
	// Proper solution:
	//Origin = Origin.TransformPointBy ( TransformCoords.Inverse().Transpose() );
	// Fast solution assuming orthogonal coordinate system:
	Origin = Origin.TransformPointBy ( TransformCoords );
	XAxis  = XAxis .TransformVectorBy( TransformCoords );
	YAxis  = YAxis .TransformVectorBy( TransformCoords );
	ZAxis  = ZAxis .TransformVectorBy( TransformCoords );
	return *this;
}
inline FCoords FCoords::operator*( const FCoords &TransformCoords ) const
{
	return FCoords(*this) *= TransformCoords;
}

//
// Transform this coordinate system by a pitch-yaw-roll rotation.
//
inline FCoords& FCoords::operator*=( const FRotation &Rot )
{
	// Apply yaw rotation.
	*this *= FCoords
	(	
		FVector( 0.0, 0.0, 0.0 ),
		FVector( +GMath.CosTab(Rot.Yaw), +GMath.SinTab(Rot.Yaw), +0.0 ),
		FVector( -GMath.SinTab(Rot.Yaw), +GMath.CosTab(Rot.Yaw), +0.0 ),
		FVector( +0.0, +0.0, +1.0 )
	);

	// Apply pitch rotation.
	*this *= FCoords
	(	
		FVector( 0.0, 0.0, 0.0 ),
		FVector( +GMath.CosTab(Rot.Pitch), +0.0, +GMath.SinTab(Rot.Pitch) ),
		FVector( +0.0, +1.0, +0.0 ),
		FVector( -GMath.SinTab(Rot.Pitch), +0.0, +GMath.CosTab(Rot.Pitch) )
	);

	// Apply roll rotation.
	*this *= FCoords
	(	
		FVector( 0.0, 0.0, 0.0 ),
		FVector( +1.0, +0.0, +0.0 ),
		FVector( +0.0, +GMath.CosTab(Rot.Roll), -GMath.SinTab(Rot.Roll) ),
		FVector( +0.0, +GMath.SinTab(Rot.Roll), +GMath.CosTab(Rot.Roll) )
	);
	return *this;
}
inline FCoords FCoords::operator*( const FRotation &Rot ) const
{
	return FCoords(*this) *= Rot;
}

inline FCoords& FCoords::operator*=( const FVector &Point )
{
	Origin -= Point;
	return *this;
}
inline FCoords FCoords::operator*( const FVector &Point ) const
{
	return FCoords(*this) *= Point;
}

//
// Detransform this coordinate system by a pitch-yaw-roll rotation.
//
inline FCoords& FCoords::operator/=( const FRotation &Rot )
{
	// Apply inverse roll rotation.
	*this *= FCoords
	(
		FVector( 0.0, 0.0, 0.0 ),
		FVector( +1.0, -0.0, +0.0 ),
		FVector( -0.0, +GMath.CosTab(Rot.Roll), +GMath.SinTab(Rot.Roll) ),
		FVector( +0.0, -GMath.SinTab(Rot.Roll), +GMath.CosTab(Rot.Roll) )
	);

	// Apply inverse pitch rotation.
	*this *= FCoords
	(
		FVector( 0.0, 0.0, 0.0 ),
		FVector( +GMath.CosTab(Rot.Pitch), +0.0, -GMath.SinTab(Rot.Pitch) ),
		FVector( +0.0, +1.0, -0.0 ),
		FVector( +GMath.SinTab(Rot.Pitch), +0.0, +GMath.CosTab(Rot.Pitch) )
	);

	// Apply inverse yaw rotation.
	*this *= FCoords
	(
		FVector( 0.0, 0.0, 0.0 ),
		FVector( +GMath.CosTab(Rot.Yaw), -GMath.SinTab(Rot.Yaw), -0.0 ),
		FVector( +GMath.SinTab(Rot.Yaw), +GMath.CosTab(Rot.Yaw), +0.0 ),
		FVector( -0.0, +0.0, +1.0 )
	);
	return *this;
}
inline FCoords FCoords::operator/( const FRotation &Rot ) const
{
	return FCoords(*this) /= Rot;
}

inline FCoords& FCoords::operator/=( const FVector &Point )
{
	Origin += Point;
	return *this;
}
inline FCoords FCoords::operator/( const FVector &Point ) const
{
	return FCoords(*this) /= Point;
}

//
// Transform this coordinate system by a scale.
// Note: Will return coordinate system of opposite handedness if
// Scale.X*Scale.Y*Scale.Z is negative.
//
inline FCoords& FCoords::operator*=( const FScale &Scale )
{
	// Apply sheering.
	FLOAT   Sheer      = FSheerSnap( Scale.SheerRate );
	FCoords TempCoords = GMath.UnitCoords;
	switch( Scale.SheerAxis )
	{
		case SHEER_XY:
			TempCoords.XAxis.Y = Sheer;
			break;
		case SHEER_XZ:
			TempCoords.XAxis.Z = Sheer;
			break;
		case SHEER_YX:
			TempCoords.YAxis.X = Sheer;
			break;
		case SHEER_YZ:
			TempCoords.YAxis.Z = Sheer;
			break;
		case SHEER_ZX:
			TempCoords.ZAxis.X = Sheer;
			break;
		case SHEER_ZY:
			TempCoords.ZAxis.Y = Sheer;
			break;
		default:
			break;
	}
	*this *= TempCoords;

	// Apply scaling.
	TempCoords = GMath.UnitCoords;
	TempCoords.XAxis.X *= Scale.Scale.X;
	TempCoords.YAxis.Y *= Scale.Scale.Y;
	TempCoords.ZAxis.Z *= Scale.Scale.Z;
	*this *= TempCoords;

	return *this;
}
inline FCoords FCoords::operator*( const FScale &Scale ) const
{
	return FCoords(*this) *= Scale;
}

//
// Detransform a coordinate system by a scale.
//
inline FCoords& FCoords::operator/=( const FScale &Scale )
{
	// Deapply scaling.
	FCoords TempCoords = GMath.UnitCoords;
	if( Scale.Scale.X != 0 ) TempCoords.XAxis.X /= Scale.Scale.X;
	if( Scale.Scale.Y != 0 ) TempCoords.YAxis.Y /= Scale.Scale.Y;
	if( Scale.Scale.Z != 0 ) TempCoords.ZAxis.Z /= Scale.Scale.Z;
	*this *= TempCoords;

	// Deapply sheering.
	TempCoords  = GMath.UnitCoords;
	FLOAT Sheer = FSheerSnap( Scale.SheerRate );
	switch( Scale.SheerAxis )
	{
		case SHEER_XY:
			TempCoords.XAxis.Y = -Sheer;
			break;
		case SHEER_XZ:
			TempCoords.XAxis.Z = -Sheer;
			break;
		case SHEER_YX:
			TempCoords.YAxis.X = -Sheer;
			break;
		case SHEER_YZ:
			TempCoords.YAxis.Z = -Sheer;
			break;
		case SHEER_ZX:
			TempCoords.ZAxis.X = -Sheer;
			break;
		case SHEER_ZY:
			TempCoords.ZAxis.Y = -Sheer;
			break;
		default: // SHEER_NONE
			break;
	}
	*this *= TempCoords;

	return *this;
}
inline FCoords FCoords::operator/( const FScale &Scale ) const
{
	return FCoords(*this) /= Scale;
}

/*-----------------------------------------------------------------------------
	Random numbers.
-----------------------------------------------------------------------------*/

//
// Random number from 0.0 to 1.0.
//
inline FLOAT frand()
{
	return (1.0 / RAND_MAX) * rand();
}

//
// Compute pushout of a box from a plane.
//
inline FLOAT FBoxPushOut( FVector Normal, FVector Size )
{
	return Abs(Normal.X*Size.X) + Abs(Normal.Y*Size.Y) + Abs(Normal.Z*Size.Z);
}

/*-----------------------------------------------------------------------------
	Advanced geometry.
-----------------------------------------------------------------------------*/

//
// Find the intersection of an infinite line (defined by two points) and
// a plane.  Assumes that the line and plane do indeed intersect; you must
// make sure they're not parallel before calling.
//
inline FVector FLinePlaneIntersection
(
	const FVector &Point1,
	const FVector &Point2,
	const FVector &PlaneOrigin,
	const FVector &PlaneNormal
)
{
	return
		Point1
	+	(Point2-Point1)
	*	(((PlaneOrigin - Point1)|PlaneNormal) / ((Point2 - Point1)|PlaneNormal));
}
inline FVector FLinePlaneIntersection
(
	const FVector &Point1,
	const FVector &Point2,
	const FPlane  &Plane
)
{
	return
		Point1
	+	(Point2-Point1)
	*	((Plane.W - (Point1|Plane))/((Point2 - Point1)|Plane));
}

/*-----------------------------------------------------------------------------
	FPlane functions.
-----------------------------------------------------------------------------*/

//
// Compute intersection point of three planes.
// Return 1 if valid, 0 if infinite.
//
inline int FIntersectPlanes3( FVector &I, const FPlane &P1, const FPlane &P2, const FPlane &P3 )
{
	guard(FIntersectPlanes3);

	// Compute determinant, the triple product P1|(P2^P3)==(P1^P2)|P3.
	FLOAT Det = (P1 ^ P2) | P3;
	if( Square(Det) < Square(0.001) )
	{
		// Degenerate.
		I = FVector(0,0,0);
		return 0;
	}
	else
	{
		// Compute the intersection point, guaranteed valid if determinant is nonzero.
		I = (P1.W*(P2^P3) + P2.W*(P3^P1) + P3.W*(P1^P2)) / Det;
	}
	return 1;
	unguard;
}

//
// Compute intersection point and direction of line joining two planes.
// Return 1 if valid, 0 if infinite.
//
inline FIntersectPlanes2( FVector &I, FVector &D, const FPlane &P1, const FPlane &P2 )
{
	guard(FIntersectPlanes2);

	// Compute line direction, perpendicular to both plane normals.
	D = P1 ^ P2;
	FLOAT DD = D.SizeSquared();
	if( DD < Square(0.001) )
	{
		// Parallel or nearly parallel planes.
		D = I = FVector(0,0,0);
		return 0;
	}
	else
	{
		// Compute intersection.
		I = (P1.W*(P2^D) + P2.W*(D^P1)) / DD;
		D.Normalize();
		return 1;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FRotation functions.
-----------------------------------------------------------------------------*/

//
// Convert a rotation into a vector facing in its direction.
//
inline FVector FRotation::Vector()
{
	return (GMath.UnitCoords / *this).XAxis;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNMATH
