include (FetchContent)
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
)

FetchContent_MakeAvailable(yaml-cpp)


add_executable(CrashClosing
	main.cpp
	loop.cpp
  tray.cpp
)

target_link_libraries(CrashClosing PUBLIC yaml-cpp::yaml-cpp)