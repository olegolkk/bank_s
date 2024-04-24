library_name = ../libace.a
USER_PATH := /s/ls4/groups/g0128/hdf5-install/CMake-hdf5-1.12.1/build/HDF5-1.12.1-Linux/HDF_Group/HDF5/1.12.1/
include_dirs := include ../ENV/include $(HDF5_INCLUDE) $(USER_PATH)include "/usr/lib/x86_64-linux-gnu/hdf5/serial/include/"
# library_dirs := ../ $(HDF5_LIBS_STATIC) $(USER_PATH)lib "/usr/lib/x86_64-linux-gnu/hdf5/serial/lib/"
# library := stdc++ env hdf5 hdf5_cpp
VPATH := ./
BUILD := ./build

include_dirs := $(addprefix -I, $(include_dirs))
# library_dirs := $(addprefix -L, $(library_dirs))
# library := $(addprefix -l, $(library))

CXXFLAGS := $(CXXFLAGS) -O2 -static -std=c++17 -finline-functions -Wall -Winline -Wno-unused-variable -Wno-parentheses -Wno-sizeof-pointer-memaccess $(compile_flags)
CXX = g++

source_subdirs = ace distributions

relative_source_dirs   := $(addprefix $(VPATH), $(source_subdirs))
objects_dirs           := $(addprefix $(VPATH), $(source_subdirs))
objects_cpp            := $(patsubst $(VPATH)%, %, $(wildcard $(addsuffix /*.cpp, $(relative_source_dirs))))
objects                := $(objects_cpp:%.cpp=$(BUILD)/%.o)

ifneq ("$(path_to_phm)", "")
	objects  := $(addprefix $(path_to_phm)/, $(objects))
endif

.DEFAULT: $(library_name)

.PHONY: clean

$(library_name): $(objects)
	ar rc $@ $^
	ranlib $@

$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $< -o $@ -c $(CXXFLAGS) $(include_dirs)

clean:
	-rm -f $(objects) $(library_name)
