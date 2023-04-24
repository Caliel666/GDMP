#ifndef GDMP_GPU_RESOURCES
#define GDMP_GPU_RESOURCES

#include <memory>

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/binder_common.hpp"

#if !MEDIAPIPE_DISABLE_GPU
#include "mediapipe/gpu/gpu_shared_data_internal.h"
#endif

using namespace godot;

class MediaPipeGPUResources : public RefCounted {
		GDCLASS(MediaPipeGPUResources, RefCounted);

	protected:
		static void _bind_methods();

	public:
		MediaPipeGPUResources();
		~MediaPipeGPUResources();

#if !MEDIAPIPE_DISABLE_GPU
		std::shared_ptr<mediapipe::GpuResources> get_gpu_resources();

	private:
		std::shared_ptr<mediapipe::GpuResources> gpu_resources;
#endif
};

#endif