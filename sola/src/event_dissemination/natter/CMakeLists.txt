add_library(EventDisseminationMinhcast event_dissemination_minhcast.cpp ${SOLA_SOURCE_DIR}/include/SOLA/event_dissemination_minhcast.h event_dissemination_minhcast_impl.cpp)
target_link_libraries(EventDisseminationMinhcast
        PUBLIC
        SOLAMessage
        EventDissemination
        natter_minhcast
        minhton_core_node
        Storage
        PRIVATE
        sola_check
)
target_include_directories(EventDisseminationMinhcast PUBLIC ${SOLA_SOURCE_DIR}/include)
