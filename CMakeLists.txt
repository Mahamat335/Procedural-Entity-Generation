# CMake'in minimum sürümünü belirtiyoruz. Modern özellikler için 3.15 iyi bir başlangıç.
cmake_minimum_required(VERSION 3.15)

# Proje adını, sürümünü ve kullanılacak dilleri tanımlıyoruz.
# glad.c bir C dosyası olduğu için CXX (C++) yanına C'yi de ekliyoruz.
project(MyProject VERSION 1.0 LANGUAGES CXX C)

# C++ standardını belirliyoruz. C++17 modern ve yaygın bir seçimdir.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Oluşturulacak çalıştırılabilir dosyanın adını bir değişkene atıyoruz.
set(EXECUTABLE_NAME MyProject)

# --- Kaynak Dosyalarını Toplama ---
# src klasöründeki ve alt klasörlerindeki tüm .cpp ve .c uzantılı dosyaları
# otomatik olarak bulup SOURCES değişkenine atıyoruz.
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.c"
    "src/Shapes/*.cpp"
    "src/imgui/*.cpp"
)

# --- Çalıştırılabilir Hedef Oluşturma ---
# Yukarıda topladığımız kaynak dosyalarından bir çalıştırılabilir dosya (executable) oluşturuyoruz.
add_executable(${EXECUTABLE_NAME} ${SOURCES})


# --- HEDEFE ÖZEL AYARLAR (DAHA MODERN VE GÜVENLİ YÖNTEM) ---

# Derleyicinin başlık (.h) dosyalarını bulacağı klasörleri SADECE bu hedefe özel olarak belirtiyoruz.
target_include_directories(${EXECUTABLE_NAME}
    PRIVATE
    "${CMAKE_SOURCE_DIR}/Header Files"
    "${CMAKE_SOURCE_DIR}/Libraries/include"
    "${CMAKE_SOURCE_DIR}/Libraries/include/imgui"  # <-- EKSİK OLAN VE HATAYI ÇÖZEN SATIR
)

# Linker'ın kütüphane (.lib) dosyalarını bulacağı klasörü belirtiyoruz.
target_link_directories(${EXECUTABLE_NAME}
    PRIVATE
    "${CMAKE_SOURCE_DIR}/Libraries/lib"
)

# Oluşturduğumuz çalıştırılabilir hedefin hangi kütüphanelere bağlanacağını belirtiyoruz.
target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE
    glfw3       # GLFW kütüphanemiz
    opengl32    # Standart OpenGL kütüphanesi
    gdi32       # Windows GDI (Grafik Arayüzü) kütüphanesi
    shell32     # Windows Shell API kütüphanesi
)

# --- Kaynak (Resource) Dosyalarını Kopyalama ---
# Bu komut, projeniz her derlendiğinde 'resources' klasörünü
# çalıştırılabilir dosyanın (.exe) yanına kopyalar.
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources"
    COMMENT "Kaynak dosyaları (shaders, textures) build dizinine kopyalanıyor..."
)