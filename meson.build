project('ch8', 'cpp', default_options: ['cpp_std=c++17'])

sdl2_dep = dependency('sdl2')
imgui_dep = dependency('imgui')

sources = [
  'src/ch8_audio.cpp',
  'src/ch8_cpu.cpp',
  'src/ch8_display.cpp',
  'src/ch8_keyboard.cpp',
  'src/ch8_log.cpp',
  'src/ch8_opcodes.cpp',
  'src/main.cpp'
]

executable('ch8', sources, dependencies: [sdl2_dep, imgui_dep])
