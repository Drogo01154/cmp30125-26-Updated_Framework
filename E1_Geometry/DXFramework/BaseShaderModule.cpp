#include "BaseShaderModule.h"

//Constructor
BaseShaderModule::BaseShaderModule(ID3D11Device* device, HWND hwnd) : renderer(device), hwnd(hwnd) {
}