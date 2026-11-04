# Controle de Missao de AUV com BehaviorTree.CPP e Groot2 (ROS 2 Humble / C++)

Repositório com a implementação em C++ utilizando ROS 2 Humble para o controle de missão de um Veículo Autônomo Subaquático (AUV) através de árvores de comportamento (*Behavior Trees*) com BehaviorTree.CPP e monitoramento em tempo real via Groot2 (`ament_cmake`).

## Estrutura do Projeto

- `src/executar_teste3.cpp`: Nó principal em C++ do ROS 2 que inicializa a fábrica de árvores, gerencia a *Blackboard* (memória compartilhada) com as variáveis de estado e publica o status de execução na porta `1667` para o Groot2.
- `untitled_1.xml`: Ficheiro XML contendo a estrutura lógica da árvore de comportamento (com ramificações de Missão, Fallback principal e tratamento de Erro/Emergência).
- `package.xml` e `CMakeLists.txt`: Configurações e dependências do pacote ROS 2 para compilação via `colcon`.

## Especificações de Processamento e Lógica

- **Arquitetura de Resiliência:** Utiliza um nó do tipo `Fallback` na raiz para alternar automaticamente entre a execução da missão principal e o ramo de contingência/emergência em caso de falha.
- **Blackboard e Variáveis de Estado:** Gerencia variáveis chave para decisões de missão, tais como `role`, `at_bins_pipe`, `is_role_bin`, `light_active`, `drop_marker`, `engage_magnet`, `surface_auv` e `shutdown_auv`.
- **Monitoramento Remoto:** Integração nativa com o Groot2 Publisher na porta TCP `1667` para visualização gráfica do estado de cada nó da árvore em tempo real.

## Como Compilar e Executar

No seu workspace ROS 2 (`~/ros2_ws`):

```bash
cd ~/ros2_ws
colcon build --packages-select auv_teste_3 --parallel-workers 1
source install/setup.bash
ros2 run auv_teste_3 executar_teste3
