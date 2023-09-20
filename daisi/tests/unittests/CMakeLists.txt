add_executable(NatterStaticCalculationTest "")
target_sources(NatterStaticCalculationTest
        PRIVATE
        natter/static_calculation_test.cpp
)
target_link_libraries(NatterStaticCalculationTest
        PRIVATE
        Catch2::Catch2WithMain
        daisi_natter_static_network_calculation
)

add_executable(CPPSAmrMobilityHelperTest)
target_sources(CPPSAmrMobilityHelperTest
        PRIVATE
        cpps/amr/amr_mobility_helper_test.cpp
)
target_link_libraries(CPPSAmrMobilityHelperTest PRIVATE Catch2::Catch2WithMain daisi_cpps_amr_amr_mobility_helper)

add_executable(DaisiDatastructureDirectedGraphTest "")
target_sources(DaisiDatastructureDirectedGraphTest
        PRIVATE
        datastructure/directed_graph_test.cpp
)
target_link_libraries(DaisiDatastructureDirectedGraphTest
        PRIVATE
        Catch2::Catch2WithMain
        daisi_datastructure_directed_graph
)

add_executable(DaisiDatastructureWeightedDirectedGraphTest "")
target_sources(DaisiDatastructureWeightedDirectedGraphTest
        PRIVATE
        datastructure/weighted_directed_graph_test.cpp
)
target_link_libraries(DaisiDatastructureWeightedDirectedGraphTest
        PRIVATE
        Catch2::Catch2WithMain
        daisi_datastructure_weighted_directed_graph
)

add_executable(DaisiDatastructureSimpleTemporalNetworkTest "")
target_sources(DaisiDatastructureSimpleTemporalNetworkTest
        PRIVATE
        datastructure/simple_temporal_network_test.cpp
)
target_link_libraries(DaisiDatastructureSimpleTemporalNetworkTest
        PRIVATE
        Catch2::Catch2WithMain
        daisi_datastructure_simple_temporal_network
)

add_executable(DaisiCppsOrderManagementStnOrderManagement "")
target_sources(DaisiCppsOrderManagementStnOrderManagement
        PRIVATE
        cpps/order_management/stn_order_management_test.cpp
)
target_link_libraries(DaisiCppsOrderManagementStnOrderManagement
        PRIVATE
        Catch2::Catch2WithMain
        ns3::libcore
        daisi_cpps_logical_order_management_stn_order_management
        daisi_cpps_amr_physical_material_flow_functionality_mapping
)

add_executable(DaisiCppsOrderManagementSimpleOrderManagement "")
target_sources(DaisiCppsOrderManagementSimpleOrderManagement
        PRIVATE
        cpps/order_management/simple_order_management_test.cpp
)
target_link_libraries(DaisiCppsOrderManagementSimpleOrderManagement
        PRIVATE
        Catch2::Catch2WithMain
        ns3::libcore
        daisi_cpps_logical_order_management_simple_order_management
        daisi_cpps_amr_physical_material_flow_functionality_mapping
)

add_executable(DaisiCppsLogicalAuctionParticipantState "")
target_sources(DaisiCppsLogicalAuctionParticipantState
        PRIVATE
        cpps/logical/auction_participant_state_test.cpp
)
target_link_libraries(DaisiCppsLogicalAuctionParticipantState
        PRIVATE
        Catch2::Catch2WithMain
        daisi_cpps_logical_algorithms_assignment_auction_participant_state
)
