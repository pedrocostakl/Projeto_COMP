# deiGo - COMP2024

Autores:
- Marco Manuel Almeida e Silva - 2021211653
- Pedro Sousa da Costa - 2022220304

### Gramática

400 palavras.

### Estruturas de Dados (AST e tabela de símbolos)

400 palavras.

A AST é principalmente composta por nós *node_t*. Estes nós possuem campos que guardam informações importantes sobre o token passado pelo lexer tais como linha, coluna, etc. Cada nó possui como campo uma lista ligada *node_list_t* que permite iterar sobre os seus filhos, caso estes existam.
As principais funções usadas para a criação da AST são a função *newnode* que é tipicamente usada para nós folha e que usa uma estrutura auxiliar pass para guardar informações como linha e coluna do token. A função *newcategory* cria nós com uma categoria específica e é tipicamente usado para statements. A função *newintermediate* cria nós intermédios que são usados para ligar vários nós, estes nós depois não serão impressos na impressão da AST.

Através da gramática é possível construir a AST ao usar a função addchild que adiciona um nó filho a um nó pai, se o nó filho é um intermediate node, os seus filhos são adicionados ao nó pai diretamente.

As tabelas de símbolos usam a estrutura de dados *symbol_list_t*  que é uma lista ligada que possui informações sobre os símbolos e a sua scope. A *global_symbol_table* é inicializada na função *check_program*. As tabelas de símbolos locais são creadas pela função *enter_scope*. Para adicionarmos novos símbolos usa-se *insert_symbol* que retorna NULL se o símbolo já existe. Iniciamos pela função *check_program* que itera pela AST para encontrar variáveis globais e funções. De seguida preenche-se a *global_symbol_table* e processamos cada função recursivamente para preencher a sua tabela de símbolos locais e averiguar a existência de erros semânticos.

De seguida é usada a função *show_symbol_table* para imprimir as tabelas e a função *show_node* que percorre a AST desde a raiz até às suas folhas, anotando-a (caso a flag esteja ativa) e ignorando os intermediate_nodes.

### Geração de Código

Iniciamos pela declaração das funções de *printf()* e *atoi()* da linguagem C. Seguidamente são declaradas as variáveis globais constantes de cadeias de caracteres para cada um dos casos de print: inteiros, floats, boolean (true ou false) e literais de strings. O próximo passo será percorrer a AST do programa e declarar variáveis globais constantes para cada literal de string utilizada, garantindo que não haverá literais declaradas mais do que uma vez. O nome da variável será uma hash correspondente à cadeia de caracteres que será guardada no node da AST para que seja possível verificar se a hash já foi utilizada (ou seja, se já foi declarada). É também decalarado uma variável para guardar o endereço dos argumentos de entrada do programa, para que seja possível aceder aos args em qualquer parte do programa. É importante referir que todas as variáveis globais referidas anteriormente que não fazem parte do programa escrito utilizam um "." como prefixo no seu nome, evitando possíveis conflitos com os nomes das variáveis do programa.

Para gerar o código do programa temos de percorrer a tabela de símbolos globais duas vezes. A primeira iteração é feita para declarar apenas todas as variáveis globais presentes no programa. Já a segunda iteração é feita para definir todas as funções presentes no programa. Foi utilizado o prefixo "_" no nome das funções para evitar o conflito entre o main definido no programa e o main do ponto de entrada. No início da função é alocado memória na stack para os parâmetros e para as variáveis locais da função. Os endereços são guardados em registos com o nome do parâmetro/variável, adicionado o prefixo "." no caso dos parâmetros para evitar o conflito com o registo da signature. No caso das branches (if/else, for loops, print de booleans) foi necessário implementar um sistema de criação de labels únicas juntando um tipo e o index, este que é reiniciado para cada função. Todas as funções apresentam um statement de retorno adicionado por defeito, caso não seja explícito no programa.

Por fim, é definido o ponto de entrada *main*. O primeiro passo será guardar os argumentos de entrada na variável global e depois haverá dois casos possíveis. Caso uma função main se encontre na tabela de símbolos global será feita uma chamada a essa função guardando o valor de retorno num registo, devolvendo esse mesmo valor de seguida. Caso não encontre a função main, será apenas retornado o valor 0, possibilitanto a compilação de programas vazios.
