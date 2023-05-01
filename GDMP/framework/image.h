#ifndef GDMP_IMAGE
#define GDMP_IMAGE

#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

#include "mediapipe/framework/formats/image.h"
#if !MEDIAPIPE_DISABLE_GPU
#include "mediapipe/gpu/gpu_buffer.h"
#endif

#include "GDMP/framework/packet.h"

using namespace godot;

class MediaPipeImage : public RefCounted {
		GDCLASS(MediaPipeImage, RefCounted)

	private:
		mediapipe::Image image;

	protected:
		static void _bind_methods();

	public:
		MediaPipeImage();
		MediaPipeImage(Ref<godot::Image> image);
		MediaPipeImage(mediapipe::Image image);
		MediaPipeImage(mediapipe::ImageFrameSharedPtr image_frame);
#if !MEDIAPIPE_DISABLE_GPU
		MediaPipeImage(mediapipe::GpuBuffer gpu_buffer);
#endif
		~MediaPipeImage();

		// Check if the image is a GPU image.
		bool is_gpu_image();

		// Get a copy of godot::Image.
		// Convert GPU image to CPU when necessary.
		Ref<godot::Image> get_godot_image();
		// Copy the data of godot::Image to the image.
		void set_godot_image(Ref<godot::Image> image);

		// Make a mediapipe::Image packet.
		Ref<MediaPipePacket> make_packet();
		// Make a mediapipe::ImageFrame packet.
		// For making mediapipe::GpuBuffer packet, refer to MediaPipeGPUHelper::make_gpu_buffer_packet.
		Ref<MediaPipePacket> make_image_frame_packet();

		// Set the image from a packet that contains either mediapipe::Image, mediapipe::ImageFrame or mediapipe::GpuBuffer.
		void set_image_from_packet(Ref<MediaPipePacket> packet);

		// Get underlying mediapipe::Image.
		mediapipe::Image get_mediapipe_image();
};

#endif