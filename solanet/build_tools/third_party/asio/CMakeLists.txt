add_library(asio INTERFACE ${SolaNet_SOURCE_DIR}/third_party/asio/asio/include/asio.hpp)
target_compile_options(asio INTERFACE -DASIO_STANDALONE)
target_include_directories(asio INTERFACE ${SolaNet_SOURCE_DIR}/third_party/asio/asio/include/)
