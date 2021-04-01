#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#define CL_TARGET_OPENCL_VERSION 200

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

std::string status2str(cl_int status) {
  if (status == CL_SUCCESS) {
    return "CL_SUCCESS";
  } else if (status == CL_INVALID_VALUE) {
    return "CL_​INVALID_​VALUE";
  } else if (status == CL_OUT_OF_HOST_MEMORY) {
    return "CL_​OUT_​OF_​HOST_​MEMORY";
  } else {
    throw std::runtime_error("Unknown status: " + std::to_string(status));
  }
}

#define CHECK_CL(status, str)                                                  \
  if (status != CL_SUCCESS) {                                                  \
    std::cout << "OpenCL Failure: " << str << status2str(status) << "\n"       \
              << std::flush;                                                   \
    exit(1);                                                                   \
  }


std::string get_platform_info(cl_platform_id platform, cl_platform_info params) {
  size_t buffer_size;
  CHECK_CL(clGetPlatformInfo(platform, params, 0, NULL, &buffer_size), "Can't get platform info buffer size.");
  char *buffer = (char*)malloc(buffer_size);
  CHECK_CL(clGetPlatformInfo(platform, params, buffer_size, buffer, NULL), "Can't get platform info.");
  std::string ret = std::string(buffer);
  free(buffer);
  return ret;
}


std::string get_device_info(cl_device_id device, cl_device_info params) {
  size_t buffer_size;
  CHECK_CL(clGetDeviceInfo(device, params, 0, NULL, &buffer_size), "Can't get device info buffer size.");
  char* buffer = (char*)malloc(buffer_size);
  CHECK_CL(clGetDeviceInfo(device, params, buffer_size, buffer, NULL), "Can't get device info.");
  std::string ret = std::string(buffer);
  if (params == CL_DEVICE_MAX_COMPUTE_UNITS || params == CL_DEVICE_MAX_CLOCK_FREQUENCY) {
    cl_uint num = *((cl_uint*)buffer);
    std::ostringstream oss;
    oss << num;
    ret = oss.str();
  } else if (params == CL_DEVICE_GLOBAL_MEM_SIZE) {
    cl_ulong num = *((cl_ulong*)buffer);
    num /= 1e9;
    std::ostringstream oss;
    oss << num;
    ret = oss.str();
  }
  free(buffer);
  return ret;
}


int main(int argc, char *argv[]) {
  cl_uint num_platforms;
  CHECK_CL(clGetPlatformIDs(0, NULL, &num_platforms), "Can't get platform ids");
  cl_platform_id platforms[num_platforms];
  CHECK_CL(clGetPlatformIDs(num_platforms, platforms, NULL), "Can't get platforms");

  for (size_t i = 0; i < num_platforms; ++i) {
      cl_platform_id current_id = platforms[i];
      std::string name = get_platform_info(current_id, CL_PLATFORM_NAME);
      std::cout << "Platform #" << i << ":\n" << "  Name:" << "\t" << name << "\n";

      cl_uint num_devices;
      CHECK_CL(clGetDeviceIDs(current_id, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices), "Can't get device number.");
      cl_device_id devices[num_devices];
      CHECK_CL(clGetDeviceIDs(current_id, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL), "Can't get device ids.");
      for (size_t j = 0; j < num_devices; ++j) {
        std::cout << "\tDevice #" << j << ":\n";
        std::string device_name = get_device_info(devices[j], CL_DEVICE_NAME);
        std::cout << "\t\tName: " << device_name << "\n";
        std::string opencl_version = get_device_info(devices[j], CL_DEVICE_OPENCL_C_VERSION);
        std::cout << "\t\tOpenCL Version: " << opencl_version << "\n";
        std::string max_compute_units = get_device_info(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS);
        std::cout << "\t\tMax Compute Units: " << max_compute_units << "\n";
        std::string max_frequency = get_device_info(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY);
        std::cout << "\t\tMax Frequency: " << max_frequency << " MHz\n";
        std::string global_mem = get_device_info(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE);
        std::cout << "\t\tGlobal Memory Size: " << global_mem << " GB\n";
      }
  }
  return 0;
}