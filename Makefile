#
# Makefile
#
CC 				= gcc  #C语言编译器
CXX             = g++  #C++编译器

#CC 				= arm-linux-gcc
LVGL_DIR_NAME 	?= lvgl
LVGL_DIR 		?= .

#去掉所有警告参数
# WARNINGS		:= -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith \
# 					-fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits \
# 					-Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security \
# 					-Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body \
# 					-Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -std=gnu99
CFLAGS 			?= -O3 -g0 -I$(LVGL_DIR)/ -I/usr/include/opencv4 $(WARNINGS) 
LDFLAGS 		?= -L/usr/lib/x86_64-linux-gnu -lm -lSDL2 -lfreetype -lpthread -lpng16 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_objdetect -lopencv_videoio -lopencv_face -lalibabacloud-oss-cpp-sdk -lcurl -lssl -lcrypto -lz
#LDFLAGS 		?= -lm -lpthread

BIN 			= main
BUILD_DIR 		= ./build
BUILD_OBJ_DIR 	= $(BUILD_DIR)/obj
BUILD_BIN_DIR 	= $(BUILD_DIR)/bin

prefix 			?= /usr
bindir 			?= $(prefix)/bin

#Collect the files to compile
MAINSRC          =  ./main.cpp \
					./my_demo/src/screens/home.cpp \
					./my_demo/src/common/staff.cpp \
					./my_demo/src/server/TCP_server.cpp \
					./my_demo/src/common/signInfo.cpp \
					./my_demo/src/oss/OssDownloader.cpp \

#添加ui文件
# CSRCS += $(shell find ./ui -type f -name '*.c')
# 添加my_demo文件

CFLAGS          +=-I ./my_demo/inc/

include $(LVGL_DIR)/lvgl/lvgl.mk

CSRCS 			+=$(LVGL_DIR)/mouse_cursor_icon.c 

OBJEXT 			?= .o

AOBJS 			= $(ASRCS:.S=$(OBJEXT))
COBJS 			= $(CSRCS:.c=$(OBJEXT))
MAINOBJ 		= $(MAINSRC:.cpp=$(OBJEXT)) 

SRCS 			= $(ASRCS) $(CSRCS) 
OBJS 			= $(AOBJS) $(COBJS)  $(MAINOBJ)
TARGET 			= $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst ./%, %, $(OBJS)))

## MAINOBJ -> OBJFILES 
SRCXX  =  $(MAINSRC)   #C++源文件
OBJCXX =  $(MAINOBJ)   #C++ .o文件


all: default

$(BUILD_OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX)  $(CFLAGS) -c $< -o $@
	@echo "$(CXX) $<"

$(BUILD_OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "$(CC) $<"

$(BUILD_OBJ_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "$(CC) $<"

default: $(TARGET)
	@mkdir -p $(dir $(BUILD_BIN_DIR)/)
	$(CXX) -o $(BUILD_BIN_DIR)/$(BIN) $(TARGET) $(LDFLAGS)

clean: 
	rm -rf $(BUILD_DIR)

install:
	install -d $(DESTDIR)$(bindir)
	install $(BUILD_BIN_DIR)/$(BIN) $(DESTDIR)$(bindir)

uninstall:
	$(RM) -r $(addprefix $(DESTDIR)$(bindir)/,$(BIN))