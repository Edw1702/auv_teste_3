#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "rclcpp/rclcpp.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/loggers/groot2_publisher.h"

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("auv_teste_3_node");

    BT::BehaviorTreeFactory factory;

    // 1. Cria a Blackboard com os EXATOS nomes que estão na sua imagem do Groot2
    auto blackboard = BT::Blackboard::create();
    blackboard->set<std::string>("role", "survey_repair");
    blackboard->set<std::string>("at_bins_pipe", "false"); // Mude para "true" quando detectar as Bins
    blackboard->set<std::string>("is_role_bin", "true");   // "true" = Bin de 800 pts | "false" = 300 pts
    blackboard->set<std::string>("light_active", "true");  // "true" se o anel de LED estiver aceso
    blackboard->set<std::string>("drop_marker", "false");
    blackboard->set<std::string>("engage_magnet", "false");
    blackboard->set<std::string>("surface_auv", "false");
    blackboard->set<std::string>("shutdown_auv", "false");

    // 2. Carrega o arquivo XML salvo pelo seu Groot2 usando caminho absoluto
    std::string xml_path = "/home/eduardo/ros2_ws/src/auv_teste_3/untitled_1.xml";
    
    RCLCPP_INFO(node->get_logger(), "A carregar a árvore de comportamento de: %s", xml_path.c_str());

    int contador_ticks = 0;
    BT::NodeStatus status = BT::NodeStatus::RUNNING;

    try {
        // Cria a árvore utilizando a fábrica, o caminho e a blackboard criada
        BT::Tree tree = factory.createTreeFromFile(xml_path, blackboard);

        // 3. Habilita monitoramento em tempo real no Groot2 (porta 1667)
        BT::Groot2Publisher publisher(tree, 1667);
        RCLCPP_INFO(node->get_logger(), "Árvore carregada! Conecte o Groot2 Monitor na porta 1667.");

        rclcpp::Rate rate(20); // 20 Hz
        while (rclcpp::ok() && status == BT::NodeStatus::RUNNING)
        {
            contador_ticks++;

            // SIMULAÇÃO DE SENSOR: Após 20 ciclos (~1 segundo), simula que o AUV chegou nas Bins
            if (contador_ticks == 20) {
                RCLCPP_INFO(node->get_logger(), "[VISÃO] Tubulação das Bins detectada!");
                blackboard->set<std::string>("at_bins_pipe", "true");
            }

            // Executa 1 ciclo (tick) da árvore do Groot2
            status = tree.tickOnce();

            // --- LEITURA DOS COMANDOS DA MISSÃO ---
            std::string drop_cmd = "false";
            if (blackboard->get<std::string>("drop_marker", drop_cmd) && drop_cmd == "true")
            {
                RCLCPP_INFO(node->get_logger(), "[ATUADOR] Soltando marcador na Bin (drop_marker = true)!");
                blackboard->set<std::string>("drop_marker", "false"); // Reseta para o próximo ciclo
            }

            std::string magnet_cmd = "false";
            if (blackboard->get<std::string>("engage_magnet", magnet_cmd) && magnet_cmd == "true")
            {
                RCLCPP_INFO_THROTTLE(node->get_logger(), *node->get_clock(), 250,
                                     "[ÍMÃ] Aproximando ímã do sensor magnético por 500ms...");
            }

            // --- LEITURA DOS COMANDOS DE EMERGÊNCIA (RAMO DE ERRO) ---
            std::string surface_cmd = "false";
            if (blackboard->get<std::string>("surface_auv", surface_cmd) && surface_cmd == "true")
            {
                RCLCPP_WARN_THROTTLE(node->get_logger(), *node->get_clock(), 1000,
                                     "[EMERGÊNCIA] Falha na missão! Subindo para a superfície (surface_auv = true)...");
            }

            std::string shutdown_cmd = "false";
            if (blackboard->get<std::string>("shutdown_auv", shutdown_cmd) && shutdown_cmd == "true")
            {
                RCLCPP_ERROR(node->get_logger(), "[DESLIGAMENTO] AUV na superfície. Desligando motores (shutdown_auv = true)!");
                break;
            }

            rclcpp::spin_some(node);
            rate.sleep();
        }

        RCLCPP_INFO(node->get_logger(), "Execução encerrada com status: %s", BT::toStr(status).c_str());

    } catch (const std::exception &e) {
        RCLCPP_ERROR(node->get_logger(), "Erro ao carregar ou executar a árvore: %s", e.what());
    }

    rclcpp::shutdown();
    return 0;
}