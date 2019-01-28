#include "aten_xla_bridge.h"

#include "tensor_impl.h"
#include "tensorflow/compiler/xla/xla_client/debug_macros.h"

namespace torch_xla {
namespace bridge {

XLATensor& GetXlaTensor(const at::Tensor& tensor) {
  TensorImpl* impl = dynamic_cast<TensorImpl*>(tensor.unsafeGetTensorImpl());
  XLA_CHECK(impl != nullptr);
  return impl->tensor();
}

std::vector<at::Tensor> XlaCreateTensorList(const at::TensorList& tensors) {
  std::vector<at::Tensor> xtensors;
  for (auto& tensor : tensors) {
    xtensors.push_back(XlaToAtenTensor(tensor));
  }
  return xtensors;
}

at::Tensor XlaToAtenTensor(const at::Tensor& tensor) {
  return GetXlaTensor(tensor).ToTensor();
}

at::Tensor XlaToAtenMutableTensor(const at::Tensor& tensor) {
  return GetXlaTensor(tensor).ToMutableTensor();
}

std::vector<at::Tensor> CreateXlaTensors(const std::vector<at::Tensor>& tensors,
                                         const Device& device) {
  std::vector<at::Tensor> xtensors;
  for (auto& tensor : tensors) {
    xtensors.push_back(CreateXlaTensor(tensor, device));
  }
  return xtensors;
}

Device XlaTensorDevice(const at::Tensor& tensor) {
  return GetXlaTensor(tensor).GetDevice();
}

Device XlaTensorDevice(const at::TensorOptions& tensor_options) {
  return Device(DeviceType::CPU, 0);
}

at::Tensor CreateXlaTensor(const at::Tensor& tensor, const Device& device) {
  XLATensor xtensor =
      XLATensor::Create(tensor, device, /*requires_grad=*/false);
  return at::Tensor(c10::intrusive_ptr<TensorImpl>::make(std::move(xtensor)));
}

}  // namespace bridge
}  // namespace torch_xla