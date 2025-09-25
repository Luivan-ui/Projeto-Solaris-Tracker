
# Documentação do Projeto: Plataforma de Controle e Análise Solar v2.0 ☀️

Este documento detalha o funcionamento, as melhorias e a estrutura do projeto de um **rastreador solar de eixo único** controlado por uma aplicação desktop. A plataforma integra hardware (Arduino) e software (WPF em C#) para monitorar e otimizar a captação de energia solar.

## 1. Visão Geral do Sistema ⚙️

O projeto é composto por três partes principais:

1.  **Código Arduino (`Arduino_Escravo_Final.ino`):** Atua como o "cérebro" do hardware, lendo os dados dos sensores de luz (LDRs) e controlando a posição do servo motor. Ele se comunica com a aplicação WPF via porta serial.
2.  **Interface de Usuário WPF (`MainWindow.xaml`):** Define a aparência e a disposição dos elementos gráficos da aplicação, como botões, texto, gráficos e abas.
3.  **Lógica da Aplicação (`MainWindow.cs`):** O código em C# que gerencia a comunicação serial, implementa o algoritmo de rastreamento, processa os dados dos sensores e atualiza a interface do usuário em tempo real.

---

## 2. Componentes e Funções Principais (C# - `MainWindow.cs`) 🖥️

O arquivo `MainWindow.cs` contém a lógica central do sistema. As seguintes seções destacam suas principais funcionalidades e melhorias.

### Algoritmo de Rastreamento Aprimorado

A função `AutoModeTimer_Tick` implementa o algoritmo de controle. A versão 2.0 introduz melhorias significativas:

* **Filtro de Média Móvel:** As leituras dos sensores LDR (sensores de luz) são suavizadas (`_ldrLeftReadings` e `_ldrRightReadings`). Isso **reduz o "tremor" do servo** causado por flutuações rápidas na iluminação (ex: nuvens passageiras), resultando em um movimento mais estável e preciso.
* **Controle Proporcional (`Kp`):** Um fator de ganho (`Kp = 0.05`) determina a agressividade do movimento do servo. Um valor menor resulta em movimentos mais suaves.
* **"Zona Morta" (`tolerancia`):** Uma tolerância de 30 pontos foi adicionada para evitar que o servo se mova para pequenas variações de luz. Isso poupa o servo motor de movimentos desnecessários e aumenta sua vida útil.
* **Proteção do Servo:** A posição calculada é limitada entre 0 e 180 graus (`Math.Clamp`), garantindo que o servo motor não tente ir além de seus limites físicos, prevenindo danos.

### Análise de Performance e Simulação 📈

* **Cálculo de Energia:** A cada segundo, a aplicação calcula a energia instantânea captada pelo rastreador e a compara com um painel fixo. O painel fixo simula uma perda de captação baseada no ângulo do servo, usando a função `Math.Cos()`.
* **Ganho de Performance:** O **ganho percentual** é calculado comparando a energia acumulada pelo painel rastreador com a do painel fixo.
* **Simulador Climático:** Um recurso de simulação permite ao usuário aplicar um fator (`_fatorClimatico`) que reduz as leituras dos sensores, simulando condições de tempo como "nublado" ou "sombra". Isso permite testar o comportamento do algoritmo sob diferentes condições sem a necessidade de luz natural real.
* **Animação do Sol:** A intensidade total de luz é usada para atualizar dinamicamente a animação de um sol na interface, com seu tamanho e brilho variando em tempo real.

### Data Logger para Análise Post-Mortem 📊

A função `LogDataToCsv` e seus complementos permitem a gravação de dados em um arquivo CSV, que pode ser usado para análises posteriores em programas como Excel ou MATLAB.

* **Formato do CSV:** A nova versão registra dados mais completos: `Timestamp, PosicaoServo, LDR_Esquerdo, LDR_Direito, IncidenciaTotal, Diferenca`. Isso fornece dados ricos para avaliar o desempenho do rastreador ao longo do tempo.
* **Opções de Gravação:** O usuário pode iniciar a gravação automática a cada 30 segundos ou salvar um ponto de dados manualmente.

---

## 3. Interface de Usuário (XAML - `MainWindow.xaml`) 🖼️

O XAML define a estrutura visual da aplicação. Ele é organizado em abas para uma melhor usabilidade:

* **Aba "Ao Vivo":** Exibe as leituras em tempo real dos sensores LDR e a posição do servo, além de um log de eventos do sistema. É aqui que o modo automático é ativado.
* **Aba "Análise de Dados":** Apresenta o "dashboard" de performance com os contadores de energia acumulada, o ganho de performance e o pico de luz. Também contém os controles para a gravação de dados.
* **Aba "Simulação e Testes":** Inclui a animação visual da intensidade solar, o menu de simulação climática e os botões de varredura manual para testes mecânicos do servo.

---

## 4. Código do Arduino (`Arduino_Escravo_Final.ino`) 🤖

O código do Arduino é simples e robusto.

* **Comunicação Bidirecional:** Ele recebe comandos da aplicação WPF (ex: `P90` para mover o servo para 90 graus) e envia continuamente os dados dos sensores LDR e a posição atual do servo.
* **Formato do Dados:** Os dados são enviados em um formato claro e consistente (`LDR_L:XXX,LDR_R:YYY,POS:ZZZ`), facilitando a análise e o processamento pelo código C#.
* **Segurança:** A função `constrain` garante que o servo não seja comandado para posições fora do intervalo de 0 a 180 graus, protegendo o motor de movimentos perigosos.

---

## 5. Explicação Detalhada do Código e Dificuldades Enfrentadas 💡

### Explicação do Código (`MainWindow.cs`)

1.  **Timers Assíncronos:** O uso de `DispatcherTimer` é crucial. O `_autoModeTimer` é responsável pela lógica de rastreamento, o `_dataLoggingTimer` pelo salvamento de dados e o `_performanceTimer` pela atualização do painel de performance. Eles operam em intervalos definidos, sem bloquear a interface de usuário, um princípio fundamental em aplicações WPF.
2.  **Tratamento de Dados:** A função `ParseAndDisplayData` é a mais importante para a comunicação. Ela recebe a string serial do Arduino, a divide em partes e atualiza as variáveis e os elementos de interface (`TextBlock`) com os valores corretos. O uso de `Dispatcher.Invoke` é necessário porque o evento `DataReceived` da porta serial é disparado em um *thread* secundário, e a interface de usuário só pode ser modificada pelo *thread* principal.
3.  **Filtro de Média Móvel:** As listas `_ldrLeftReadings` e `_ldrRightReadings` implementam o filtro. A cada nova leitura, o valor é adicionado à lista e, se a lista exceder o tamanho da janela de suavização (`SmoothingWindowSize`), o valor mais antigo é removido. A média de todos os valores na lista é então calculada para obter um valor mais estável.
4.  **Cálculo de Performance:** O cálculo do ganho de energia é feito em `PerformanceTimer_Tick`. A lógica para simular o painel fixo (`_energiaTotalFixa`) é inteligente: ela usa o ângulo do painel rastreador para determinar a perda. Quando o rastreador está desalinhado (ângulo distante de 90°), a perda para a placa fixa aumenta, refletindo a física real da captação de luz.

### Dificuldades e Soluções Encontradas

1.  **"Tremor" do Servo (Hunting):** A dificuldade mais comum em rastreadores simples é o movimento constante do servo, que oscila em torno do ponto ideal.
    * **Causa:** Pequenas flutuações nas leituras dos LDRs, causadas por ruído elétrico ou mudanças sutis na iluminação, faziam com que o algoritmo movesse o servo repetidamente.
    * **Solução:** A implementação do **filtro de média móvel** resolveu esse problema. Ao suavizar as leituras, as pequenas oscilações são ignoradas, e o servo só se move quando há uma diferença significativa e sustentada entre os sensores. A "zona morta" (`tolerancia`) também foi fundamental para criar um estado de repouso.
2.  **Comunicação Serial e UI Congelada:** Inicialmente, a leitura da porta serial bloqueava o *thread* principal, congelando a interface do usuário.
    * **Causa:** O evento `SerialPort.DataReceived` é executado em um *thread* de segundo plano. Tentar atualizar diretamente os elementos da interface (que pertencem ao *thread* da UI) causava uma exceção.
    * **Solução:** O uso do método `Dispatcher.Invoke` resolveu o problema. Ele enfileira a atualização da UI no *thread* correto, garantindo que a interface permaneça responsiva enquanto a leitura de dados ocorre em segundo plano.
3.  **Problemas com a Lógica de Varredura:** O recurso de varredura manual (`SweepButton_Click`) precisava ser aprimorado para evitar que o usuário interagisse com a UI enquanto o motor se movia.
    * **Causa:** O usuário poderia clicar em outro botão ou alternar o modo automático no meio de uma varredura, causando comportamento inesperado ou erros.
    * **Solução:** A função `SetControlsEnabled` foi criada para desativar os botões de controle durante a varredura. Além disso, o uso de `async` e `await Task.Delay` permitiu que a varredura ocorresse sem congelar a interface, proporcionando uma experiência de usuário mais fluida.

---

## 6. Componentes Utilizados na Montagem 🛠️

Para replicar o projeto, você precisará dos seguintes componentes eletrônicos:

* **Arduino:** Placa de microcontrolador (pode ser um **Uno**, **Nano** ou similar).
* **Sensores de Luz LDR:** Dois resistores dependentes de luz (LDRs) para detectar a intensidade luminosa.
* **Micro Servo Motor SG90:** Um pequeno servo motor para controlar a posição da plataforma.
* **Protoboard:** Uma placa de ensaio para montar os componentes de forma temporária e sem solda.
* **Jumpers (fios):** Fios de conexão macho-macho e/ou macho-fêmea para ligar os componentes na protoboard e no Arduino.
