#include "ari.h"
#include "nengine.h"

#include <stdlib.h>

#if NE_ARI_VULKAN

#include <vulkan/vulkan.h>

static void ne_check_vk_result(VkResult result, const i8 *file, i32 line)
{
	if (result != VK_SUCCESS)
	{
		ne_println_error("ERROR: At %s:%d Vulkan call failed", file, line);	
	}
}

#define VK_CHECK(x) ne_check_vk_result((x), __FILE__, __LINE__) 

struct NE_Ari
{
	const i8 *backend_name;
	VkInstance instance;
	VkAllocationCallbacks *allocator;
};

void ne_vk_create_instance(struct NE_Ari *ari)
{
	const VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "NEngine",
		.applicationVersion = 1,
		.pEngineName = "NEngine",
		.engineVersion = 1,
		.apiVersion = VK_VERSION_1_0
	};

	const VkInstanceCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		// TODO: Need to fill layers and and extensions
	};
	VK_CHECK(vkCreateInstance(&ci, ari->allocator, &ari->instance));
}

NE_API struct NE_Ari *ne_ari_create(void)
{
	struct NE_Ari *ari = malloc(sizeof *ari);
	ari->backend_name = "Vulkan";

	ne_vk_create_instance(ari);
	return ari;
}

NE_API void ne_ari_destroy(struct NE_Ari *ari)
{
	free(ari);
}

NE_API void ne_ari_clear(struct NE_Ari *ari, const struct NE_Vec4 *color)
{
	(void)ari;
	(void)color;
}

#endif
