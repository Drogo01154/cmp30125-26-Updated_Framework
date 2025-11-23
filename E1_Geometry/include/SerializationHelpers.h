#pragma once

#ifndef _SERIALIZATIONHELP_H_
#define _SERIALIZATIONHELP_H_

#define _USE_MATH_DEFINES
#include <directxmath.h>
#include "Nlohmann/json.hpp"

using namespace DirectX;

template<>
struct nlohmann::adl_serializer<XMFLOAT3> {
    static void to_json(nlohmann::json& j, const XMFLOAT3& f) {
        j = { {"x", f.x}, {"y", f.y}, {"z", f.z} };
    }

    static void from_json(const nlohmann::json& j, XMFLOAT3& f) {
        j.at("x").get_to(f.x);
        j.at("y").get_to(f.y);
        j.at("z").get_to(f.z);
    }
};

template<>
struct nlohmann::adl_serializer<XMFLOAT4> {
    static void to_json(nlohmann::json& j, const XMFLOAT4& f) {
        j = { {"x", f.x}, {"y", f.y}, {"z", f.z}, {"w", f.w} };
    }

    static void from_json(const nlohmann::json& j, XMFLOAT4& f) {
        j.at("x").get_to(f.x);
        j.at("y").get_to(f.y);
        j.at("z").get_to(f.z);
        j.at("w").get_to(f.w);
    }
};

template<>
struct nlohmann::adl_serializer<XMVECTOR> {
    static void to_json(nlohmann::json& j, const XMVECTOR& v) {
       
        XMFLOAT4 out;
        XMStoreFloat4(&out, v);
        j = out;
    }

    static void from_json(const nlohmann::json& j, XMVECTOR& v) {
        XMFLOAT4 in;
        j.get_to(in);
        v = XMLoadFloat4(&in);
    }
};

template<>
struct nlohmann::adl_serializer<XMMATRIX> {
    static void to_json(nlohmann::json& j, const XMMATRIX& f) {
        j = nlohmann::json::array();
        j[0] = f.r[0];
        j[1] = f.r[1];
        j[2] = f.r[2];
        j[3] = f.r[3];
    }

    static void from_json(const nlohmann::json& j, XMMATRIX& f) {
        j[0].get_to(f.r[0]);
        j[1].get_to(f.r[1]);
        j[2].get_to(f.r[2]);
        j[3].get_to(f.r[3]);
    }
};



#endif