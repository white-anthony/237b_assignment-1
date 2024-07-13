# Assignment 1

## Setup

### Intel Ubuntu (including WSL2)

```bash
# download the key to system keyring
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
| gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null

# add signed entry to apt sources and configure the APT client to use Intel repository:
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list

# install the packages for Intel OpenCL
sudo apt update
sudo apt install intel-oneapi-runtime-libs opencl-headers
```

## Lab

### Device Query
The purpose of this portion of the lab is to familiarize the student with the underlying hardware and how it relates to the OpenCL hardware model.  By extension, the program will familiarize the student with the processing resources at hand along with their capabilities.  By the end of this lab, one should understand the process of compiling and running code that will be used in subsequent modules.

The instructions for this portion of the lab is located in `lab/device_query/README.md`.

### Raytracer Sequential 
This is a ray tracer program that creates `output.png` using your CPU sequentially.  Examine `main.c` and understand what it does.  Run this program with the `time` command and report the results as part of your report for the assignment.

### Raytracer Parallel
This is a ray tracer program that creates `output.png` using your CPU as an OpenCL device. Examine `main.c` and `kernel.cl`. Run this program with the `time` command and report the results as part of your report for the assignment.

Think about why the results from `time` differ.  What can this tell you about OpenCL?  Report your thoughts in your report.

## Homework
The purpose of this lab is to introduce the student to the OpenCL API by implementing vector addition. The student will implement vector addition by writing the OpenCL kernel code as well as the associated host code.

The instructions for this portion of the homework is located in `homework/vector_add/README.md`.

## Resources
* [Documentation](https://www.khronos.org/opencl/)