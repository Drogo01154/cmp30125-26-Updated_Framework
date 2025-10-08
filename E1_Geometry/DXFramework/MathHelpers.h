#pragma once

#ifndef _MATHHELP_H_
#define _MATHHELP_H_

#include <directxmath.h>

using namespace DirectX;



XMFLOAT2 ToRadians(const XMFLOAT2& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y)
	};
}

XMFLOAT3 ToRadians(const XMFLOAT3& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y),
		XMConvertToRadians(vec.z)
	};
}

XMFLOAT4 ToRadians(const XMFLOAT4& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y),
		XMConvertToRadians(vec.z),
		XMConvertToRadians(vec.w)
	};
}

XMFLOAT2 ToDegrees(const XMFLOAT2& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y)
	};
}

XMFLOAT3 ToDegrees(const XMFLOAT3& vec) {
	return {
		XMConvertToDegrees(vec.x),
		XMConvertToDegrees(vec.y),
		XMConvertToDegrees(vec.z)
	};
}

XMFLOAT4 ToDegrees(const XMFLOAT4& vec) {
	return {
		XMConvertToDegrees(vec.x),
		XMConvertToDegrees(vec.y),
		XMConvertToDegrees(vec.z),
		XMConvertToDegrees(vec.w)
	};
}

#endif

