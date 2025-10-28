#include "BaseShaderModule.h"

BaseShaderModule::BaseShaderModule(ID3D11Device* device, HWND hwnd) : renderer(device), hwnd(hwnd) {

}

BaseShaderModule::~BaseShaderModule() {

}