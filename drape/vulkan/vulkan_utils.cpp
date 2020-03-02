#include "drape/vulkan/vulkan_utils.hpp"

#include <array>

namespace dp
{
namespace vulkan
{
namespace
{
// Sampler package.
uint8_t constexpr kWrapSModeByte = 3;
uint8_t constexpr kWrapTModeByte = 2;
uint8_t constexpr kMagFilterByte = 1;
uint8_t constexpr kMinFilterByte = 0;
}  // namespace

std::string GetVulkanResultString(VkResult result)
{
  switch (result)
  {
  case VK_SUCCESS: return "VK_SUCCESS";
  case VK_NOT_READY: return "VK_NOT_READY";
  case VK_TIMEOUT: return "VK_TIMEOUT";
  case VK_EVENT_SET: return "VK_EVENT_SET";
  case VK_EVENT_RESET: return "VK_EVENT_RESET";
  case VK_INCOMPLETE: return "VK_INCOMPLETE";
  case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
  case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
  case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
  case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
  case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
  case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
  case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
  case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
  case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
  case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
  case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
  case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
  case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
  case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
  case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
  case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
  case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
  case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
  case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
  case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
    return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
  case VK_ERROR_FRAGMENTATION_EXT: return "VK_ERROR_FRAGMENTATION_EXT";
  case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
  case VK_ERROR_OUT_OF_POOL_MEMORY_KHR: return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
  case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
  case VK_RESULT_RANGE_SIZE: return "VK_RESULT_RANGE_SIZE";
  case VK_RESULT_MAX_ENUM: return "VK_RESULT_MAX_ENUM";
  }
  UNREACHABLE();
  return "Unknown result";
}

// static
VkFormat VulkanFormatUnpacker::m_bestDepthFormat = VK_FORMAT_UNDEFINED;

// static
bool VulkanFormatUnpacker::Init(VkPhysicalDevice gpu)
{
  std::array<VkFormat, 3> depthFormats = {{VK_FORMAT_D32_SFLOAT,
                                           VK_FORMAT_X8_D24_UNORM_PACK32,
                                           VK_FORMAT_D16_UNORM}};
  VkFormatProperties formatProperties;
  for (auto depthFormat : depthFormats)
  {
    vkGetPhysicalDeviceFormatProperties(gpu, depthFormat, &formatProperties);
    if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
      m_bestDepthFormat = depthFormat;
      break;
    }
  }

  if (m_bestDepthFormat == VK_FORMAT_UNDEFINED)
  {
    LOG(LWARNING, ("Vulkan error: there is no any supported depth format."));
    return false;
  }

  vkGetPhysicalDeviceFormatProperties(gpu, Unpack(TextureFormat::DepthStencil), &formatProperties);
  if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
  {
    LOG(LWARNING, ("Vulkan error: depth-stencil format is unsupported."));
    return false;
  }

  std::array<VkFormat, 2> framebufferColorFormats = {{Unpack(TextureFormat::RGBA8),
                                                      Unpack(TextureFormat::RedGreen)}};
  for (auto colorFormat : framebufferColorFormats)
  {
    vkGetPhysicalDeviceFormatProperties(gpu, colorFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT))
    {
      LOG(LWARNING, ("Vulkan error: framebuffer format", colorFormat, "is unsupported."));
      return false;
    }
  }

  return true;
}

// static
VkFormat VulkanFormatUnpacker::Unpack(TextureFormat format)
{
  switch (format)
  {
  case TextureFormat::RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
  case TextureFormat::Alpha: return VK_FORMAT_R8_UNORM;
  case TextureFormat::RedGreen: return VK_FORMAT_R8G8_UNORM;
  case TextureFormat::DepthStencil: return VK_FORMAT_D24_UNORM_S8_UINT;
  case TextureFormat::Depth: return m_bestDepthFormat;
  case TextureFormat::Unspecified:
    CHECK(false, ());
    return VK_FORMAT_UNDEFINED;
  }
  UNREACHABLE();
}

SamplerKey::SamplerKey(TextureFilter filter, TextureWrapping wrapSMode, TextureWrapping wrapTMode)
{
  Set(filter, wrapSMode, wrapTMode);
}

void SamplerKey::Set(TextureFilter filter, TextureWrapping wrapSMode, TextureWrapping wrapTMode)
{
  SetStateByte(m_sampler, static_cast<uint8_t>(filter), kMinFilterByte);
  SetStateByte(m_sampler, static_cast<uint8_t>(filter), kMagFilterByte);
  SetStateByte(m_sampler, static_cast<uint8_t>(wrapSMode), kWrapSModeByte);
  SetStateByte(m_sampler, static_cast<uint8_t>(wrapTMode), kWrapTModeByte);
}

TextureFilter SamplerKey::GetTextureFilter() const
{
  return static_cast<TextureFilter>(GetStateByte(m_sampler, kMinFilterByte));
}

TextureWrapping SamplerKey::GetWrapSMode() const
{
  return static_cast<TextureWrapping>(GetStateByte(m_sampler, kWrapSModeByte));
}

TextureWrapping SamplerKey::GetWrapTMode() const
{
  return static_cast<TextureWrapping>(GetStateByte(m_sampler, kWrapTModeByte));
}

bool SamplerKey::operator<(SamplerKey const & rhs) const
{
  return m_sampler < rhs.m_sampler;
}
}  // namespace vulkan
}  // namespace dp
