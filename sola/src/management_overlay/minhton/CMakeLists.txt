add_library(ManagementOverlayMINHTONObj OBJECT management_overlay_minhton.cpp)
target_link_libraries(ManagementOverlayMINHTONObj
        PUBLIC
        Storage
        minhton_core_node
        PRIVATE
        minhton_core_definitions
)
target_include_directories(ManagementOverlayMINHTONObj
        PUBLIC
        ${SOLA_SOURCE_DIR}/include)

add_library(ManagementOverlayMINHTON STATIC $<TARGET_OBJECTS:ManagementOverlayMINHTONObj>)
target_link_libraries(ManagementOverlayMINHTON
        PUBLIC
        $<TARGET_OBJECTS:ManagementOverlayMINHTONObj>
)
