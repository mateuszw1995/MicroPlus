#pragma once
#include "../../Math/Camera.h"
#include "../../Math/maths.h"

Camera::Camera() {}

FPPCamera::FPPCamera() : smoother(1.0), slev(0.0), drot_pitch(0.0), drot_yaw(0.0), drot_roll(0.0), upmode(GLOBAL), strafemode(LOCAL), mode(FPP) {
	PitchLimit(false, -89.0, 89.0);
	YawLimit(false, -179.0, 179.0);
	RollLimit(false, -179.0, 179.0);
	updaterot = false;
	cm[0] = cm[1] = 0;
}

void Camera::UpdatePosition() {
	modelView.UpdatePosition();
}
void Camera::UpdateMatrix() {
	modelView.UpdateMatrix();
}

Vec3D<> Camera::GetPosition() {
	return -Position();
}
void Camera::SetPosition(const Vec3D<>& v){
	Position() = -v;
}

Vec3D<>& Camera::Position() {
	return modelView.position;
}
Vec3D<> Camera::GetDirection() const {
	return Vec3D<>
		(
		-modelView.matrix.MATRIX[2],
		-modelView.matrix.MATRIX[6],
		-modelView.matrix.MATRIX[10]);
}
Vec3D<> Camera::GetRollDir() const {
	return Vec3D<>
		(
		-modelView.matrix.MATRIX[1],
		-modelView.matrix.MATRIX[5],
		-modelView.matrix.MATRIX[9]);
}
Vec3D<> Camera::GetXAxis() const {
	return Vec3D<>
		(
		-modelView.matrix.MATRIX[0],
		-modelView.matrix.MATRIX[4],
		-modelView.matrix.MATRIX[8]);
}
Quaternion<>& Camera::Rotation() {
	return modelView.rotation;
}


FPPCamera::Mode& FPPCamera::CameraMode() {
	return mode;
}

void FPPCamera::Smooth(int f){smoother = 1.0/(double)f;}

void FPPCamera::Yaw(double adds){
	static double adds_c;
	static Quaternion<> a;
	if(adds == 0.0) return;
	updaterot = true;

	if(limits[0]) {
		adds_c = math::clamp(limit_yaw[0]-drot_yaw, limit_yaw[1]-drot_yaw, adds);
		if(adds_c != adds) {
			a = Quaternion<>(Vec3D<double>(0.0, 1.0, 0.0), adds_c);
			rot_res = mode == FPP ? rot_res*a : a*rot_res;	
			drot_yaw += adds_c;
			return;
		}
	}
	drot_yaw += adds;

	a = Quaternion<>(Vec3D<double>(0.0, 1.0, 0.0), -adds);
	rot_res = mode == FPP ? rot_res*a : a*rot_res;
	
}
void FPPCamera::Pitch(double adds){	
	static double adds_c;
	static Quaternion<> a;
	if(adds == 0.0) return;
	updaterot = true;

	if(limits[1]) {
		adds_c = math::clamp(limit_pitch[0]-drot_pitch, limit_pitch[1]-drot_pitch, adds);
		if(adds_c != adds) {
			rot_res = Quaternion<>(Vec3D<double>(1.0, 0.0, 0.0), adds_c)*rot_res;	
			drot_pitch += adds_c;
			return;
		}
	}
	drot_pitch += adds;
	rot_res = Quaternion<>(Vec3D<double>(1.0, 0.0, 0.0), -adds)*rot_res;
	
}
void FPPCamera::Roll(double adds){
	static double adds_c;
	static Quaternion<> a;
	if(adds == 0.0) return;
	updaterot = true;

	if(limits[2]) {
		adds_c = math::clamp(limit_roll[0]-drot_roll, limit_roll[1]-drot_roll, adds);
		if(adds_c != adds) {
			rot_res = Quaternion<>(Vec3D<double>(0.0, 0.0, 1.0), adds_c)*rot_res;	
			drot_roll += adds_c;
			return;
		}
	}
	drot_roll += adds;
	rot_res = Quaternion<>(Vec3D<double>(0.0, 0.0, 1.0), -adds)*rot_res;
}

void FPPCamera::ResetRotation() {
	for(int i = 0; i < 2; ++i) {
		if(cm[i]) {
			cm[i]->curve_acc = cm[i]->curve_dec = cm[i]->curve_vel = cm[i]->curve_pos = 0.0;
			cm[i]->lastres = Vec3D<double>();
			cm[i]->actres = Vec3D<double>();
			cm[i]->invflag = false;
		}
	}
	rot_res = Quaternion<>();
	drot_pitch = drot_yaw = drot_roll = 0.0;
	updaterot = true;
}
void FPPCamera::SetAccForward(double a){  
	acc = (forw_vel = -GetDirection() * a) + side_vel + up_vel;
}
void FPPCamera::SetAccStrafe(double a){  
	acc = forw_vel + (side_vel = (-GetDirection() * (strafemode == LOCAL ? -GetRollDir() : Vec3D<>(0.0, 1.0, 0.0) )).normalize() * a);
}
void FPPCamera::SetAccUp(double a){  
	acc = forw_vel + side_vel + (up_vel = (upmode == LOCAL ? GetRollDir() : Vec3D<>(0.0, -1.0, 0.0))*a);
		//(up_vel = (roll_d ? (pitch_d ? Vec3D<double>(-GetDirection()) : (rot_roll*rot_yaw).getZAxis() )*GetXAxis() : (pitch_d ? Vec3D<double>(GetRollDir()) : Vec3D<double>(0.0, -1.0, 0.0))) * a);
}
void FPPCamera::SetAcceleration(Vec3D<> a){  
	acc = forw_vel = a;
	side_vel = up_vel = Vec3D<>(0.0, 0.0, 0.0);
}

void FPPCamera::SetVelForward(double v){  
	vel = (forw_vel = -GetDirection() * v) + side_vel + up_vel;
}
void FPPCamera::SetVelStrafe(double v){  
	vel = forw_vel + (side_vel = (-GetDirection() * (strafemode == LOCAL ? -GetRollDir() : Vec3D<>(0.0, 1.0, 0.0) )).normalize() * v);
}
void FPPCamera::SetVelUp(double v){  
	vel = forw_vel + side_vel + (up_vel = (upmode == LOCAL ? GetRollDir() : Vec3D<>(0.0, -1.0, 0.0))*v);
		//(up_vel = (roll_d ? (pitch_d ? Vec3D<double>(-GetDirection()) : (rot_roll*rot_yaw).getZAxis() )*GetXAxis() : (pitch_d ? Vec3D<double>(GetRollDir()) : Vec3D<double>(0.0, -1.0, 0.0))) * v);
}
void FPPCamera::SetVelocity(Vec3D<> v){  
	vel = forw_vel = v;
	side_vel = up_vel = Vec3D<>(0.0, 0.0, 0.0);
}

Vec3D<>& FPPCamera::Acceleration() {
	return acc;
}
Vec3D<>& FPPCamera::Deceleration() {
	return dec;
}
Vec3D<>& FPPCamera::Velocity() {
	return vel;
}
FPPCamera::CurveMovement::CurveMovement() : curve(0), curve_vel(0.0), curve_pos(0.0), curve_acc(0.0), curve_dec(0.0), inverse_repeat(false), invflag(false)  {}

Vec3D<> FPPCamera::CurveMovement::Update (double framespeed) {
	static Vec3D<> ret;

	curve_vel += curve_acc;
	curve_pos += (invflag ? -1 : 1)*curve_vel*framespeed;
	if(curve_pos < 0.0) { 
		invflag = inverse_repeat ? !invflag : false;
		curve_pos =- curve_pos;
		//lastres = Vec3D<>();
	}
	else if(curve_pos > 1.0) {
		invflag = inverse_repeat ? !invflag : false;	
		curve_pos = 2.0 - curve_pos;
		//lastres = Vec3D<>();
	}

	if(curve_acc == 0.0) {
		if(abs(curve_vel) > curve_dec) 
			curve_vel += curve_vel < 0 ? curve_dec : -curve_dec;
		else curve_vel = 0.0;
	}

	actres = curve->alignInterpolate(curve_pos);
	ret = (lastres-Vec3D<>(actres.x, actres.y, actres.z))*(invflag ? -1 : 1);
	lastres = actres;
	return ret;

}
void FPPCamera::Update(double framespeed) {
	static Vec3D<> velnorm;
	static Vec3D<double> cmres;
	vel += acc;
	
	if(cm[0]) Position() += cm[0]->Update(framespeed);
	if(cm[1]) {
		cmres = cm[1]->Update(framespeed);
		Roll(-cmres.z);
		Yaw(-cmres.x);
		Pitch(cmres.y);
	}

	if(acc.magnitude_sq() == 0.0) {
		if(vel.magnitude_sq() > dec.magnitude_sq()) {
			velnorm = vel;
			velnorm.normalize();
			velnorm.x = abs(velnorm.x) * dec.x;
			velnorm.y = abs(velnorm.y) * dec.y;
			velnorm.z = abs(velnorm.z) * dec.z;
			vel.x += vel.x < 0 ? velnorm.x : -velnorm.x;
			vel.y += vel.y < 0 ? velnorm.y : -velnorm.y;
			vel.z += vel.z < 0 ? velnorm.z : -velnorm.z;
		}
		else
			vel = Vec3D<>(0.0, 0.0, 0.0);
	}

	Position() += vel*framespeed;
	

	if(updaterot) {
		smooth_quat = modelView.rotation;
		act_quat = rot_res;
		slev = 0.0;
	}
	if(smoother < 1.0 && slev < 1.0) {
		slev += smoother;

		if(slev >= 1.0) 
			modelView.matrix = modelView.rotation = smooth_quat = act_quat;
		else
			modelView.matrix = modelView.rotation = smooth_quat.nlerp(act_quat, slev);
	}
	else
	modelView.matrix = modelView.rotation = act_quat;

	
	updaterot = false;
	memcpy(modelView.matrix.MATRIX+12, &(Position() * modelView.matrix), sizeof(Vec3D<>));
}


void FPPCamera::YawLimit(bool flag, double left, double right) {
	limits[0] = flag;
	limit_yaw[0] = left;
	limit_yaw[1] = right;
}
void FPPCamera::PitchLimit(bool flag, double up, double down) {
	limits[1] = flag;
	limit_pitch[0] = up;
	limit_pitch[1] = down;
}
void FPPCamera::RollLimit(bool flag, double ccw, double cw) {
	limits[2] = flag;
	limit_roll[0] = ccw;
	limit_roll[1] = cw;
}

void FPPCamera::YawLimit(bool flag) {
	limits[0] = flag;
}
void FPPCamera::PitchLimit(bool flag) {
	limits[1] = flag;
}
void FPPCamera::RollLimit(bool flag) {
	limits[2] = flag;
}


void FPPCamera::CurveToMotion(CurveMovement* c) {
	cm[0] = c;
}

void FPPCamera::CurveToRotation(CurveMovement* c) {
	cm[1] = c;
}

FPPCamera::AxisMode& FPPCamera::UpAxis() {
	return upmode;
}

double FPPCamera::GetYaw() {
	return drot_yaw;
}
double FPPCamera::GetPitch() {
	return drot_pitch;
}
double FPPCamera::GetRoll(){
	return drot_roll;
} 








