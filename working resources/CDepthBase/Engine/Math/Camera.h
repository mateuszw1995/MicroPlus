#pragma once
#include "maths.h"

struct Camera {
	Camera();
	
	Transformation<> modelView;
	Matrix<> projection;
	
	template<class T>
	void Perspective(T fovy, T aspect, T zNear, T zFar) {
		projection.identity(); projection.perspective(fovy, aspect, zNear, zFar);
	}
	template<class T>
	void Orthographic(T left, T right, T bottom, T top, T zNear, T zFar){
		projection.identity(); projection.ortho(left, right, bottom, top, zNear, zFar);
	}
	
	Vec3D<> GetDirection() const;
	Vec3D<> GetRollDir() const;
	Vec3D<> GetXAxis() const;
	Quaternion<>& Rotation();
	
	Vec3D<> GetPosition();
	void SetPosition(const Vec3D<>& v);
	void UpdatePosition();
	void UpdateMatrix();

protected:
	Vec3D<>& Position();
};

class FPPCamera : public Camera {
	friend int DepthMain() ;
	Quaternion<> smooth_quat, act_quat, rot_res;
	double limit_yaw[2], limit_pitch[2], limit_roll[2], drot_pitch, drot_yaw, drot_roll;
	bool limits[3];
	double smoother, slev;
	Vec3D<> acc, vel, dec, forw_acc, forw_vel, side_acc, side_vel, up_acc, up_vel;

	bool updaterot;
	friend class CScene;
	using Camera::Position;

	bool inverse_repeat, invflag;
public:
	struct CurveMovement {
		math::Curve3D<>* curve;

		double curve_acc, curve_vel, curve_pos, curve_dec;
		bool inverse_repeat, invflag;

		CurveMovement();
		Vec3D<> Update(double framespeed);
	private:
		Vec3D<double> lastres, actres;
		friend class FPPCamera;

	} *cm[2];

	enum Mode {
		FLIGHT,
		FPP
	} mode;

	enum AxisMode {
		GLOBAL,
		LOCAL
	} upmode, strafemode;
	
	Mode& CameraMode();
	AxisMode& UpAxis();
	AxisMode& StrafeAxis();

	FPPCamera();

	void Smooth(int);
	
	void Yaw(double units);
	void Pitch(double units);
	void Roll(double units);
	
	void YawLimit(bool flag, double left, double right);
	void PitchLimit(bool flag, double up, double down);
	void RollLimit(bool flag, double ccw, double cw);
	void YawLimit(bool flag);
	void PitchLimit(bool flag);
	void RollLimit(bool flag);
	
	double GetYaw();
	double GetPitch();
	double GetRoll();

	void ResetRotation();
	
	void SetAccForward(double);
	void SetAccStrafe(double);
	void SetAccUp(double);
	
	Vec3D<>& Deceleration(); 
	Vec3D<>& Acceleration();
	void SetAcceleration(Vec3D<>);
	
	void SetVelForward(double);
	void SetVelStrafe(double);
	void SetVelUp(double);
	
	void SetVelocity(Vec3D<>);
	Vec3D<>& Velocity();
	
	void CurveToMotion(CurveMovement*);
	void CurveToRotation(CurveMovement*);

	void Update(double framespeed);
};

class TPPCamera : public Camera {
public:

};

class OrbitCamera : public Camera {
public:


};