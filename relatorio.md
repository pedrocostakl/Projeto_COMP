# deiGo - Compiladores 2024

Autores:
- Marco Manuel Almeida e Silva - 2021211653
- Pedro Sousa da Costa - 2022220304

### Gramática

A regra Program foi alterada para ter três produções, permitindo programas com declarations, sem qualquer declaration e também programas sem qualquer token.

A Declarations foi dividada em quatro produções permitindo a leitura de VarDeclaration seguido de mais declarações, FuncDeclaration seguida de mais declarações, ou então VarDeclaration/FuncDeclaration sozinhos, permitindo a finalização de Declaration. O VarSpec foi alterado para reconhecer IDENTIFIER Type ou IDENTIFIER seguido de COMMA VarSpec, permitindo a declaração de múltiplas variáveis na mesma VarDeclaration. O FuncDeclaration foi dividido em dois símbolos não terminais: um para header e outro para body. 

O header tem produções para reconhecer funções com ou sem parâmetros e funções com ou sem tipo especificado, ou seja, combinações dos quatro casos. A regra Parameters foi dividida numa nova regra de parameter, onde, assim como VarSpec, permite a sequência de um ou mais parâmetros. A FuncBody tem duas produções, um onde a função não tem VarsAndStatements, e outro onde tem VarsAndStatements. 

Para VarsAndStatements, foi feito algo semelhante a Declarations, onde foi dividido em produções que permitem ter VarDeclaration seguido de VarsAndStatements, Statement seguido de VarsAndStatements, mas também casos onde não é seguido de mais algum VarsAndStatements. Para casos onde o statement poderia ser apenas um SEMICOLON, foi acrescentada essa produção e outra que que gera apenas VarsAndStatements com SEMICOLON, permitindo a sequência de SEMICOLONs com entre sequências de VarDeclaration ou Statement, tratando então de todas essas possibilidades.

Statement foi implementado de maneira bastante semelhante à gramática inicial, mas foram adicionadas múltiplas produções que tratam dos casos onde símbolos são opcionais. Para os casos onde aparecem LBRACE {Statement SEMICOLON} RBRACE foi criada uma nova regra de block que permite produções LBRACE RBRACE e também LBRACE BlockStatement RBRACE, sendo BlockStatement ainda outra nova regra que permite a sequência de statements, semlhante mais uma vez ao que foi feito em VarSpec e Parameter. Para ParseArgs foi adicionada uma produção no caso de erro. Para FuncInvocation foi criada uma nova regra de func invocation expressions que permite a leitura de várias expressions entre LBRACE e RBRACE. FuncInvocation tem então três produções, uma para ausência de argumentos, uma com o novo símbolo não terminal func invocation expressions e uma no caso de erro nos argumentos.

A regra Expr é quase idêntica à gramática original, sendo apenas adicionado o caso de erro entre LPAR RPAR e a precedência para os operadores MINUS e PLUS unários.

Por fim, a regra Type também é idêntica à gramática original.

### Estruturas de Dados (AST e tabela de símbolos)

A AST é principalmente composta por nós *node_t*. Estes nós possuem campos que guardam informações importantes sobre o token passado pelo lexer tais como linha, coluna, etc. Cada nó possui como campo uma lista ligada *node_list_t* que permite iterar sobre os seus filhos, caso estes existam.
As principais funções usadas para a criação da AST são a função *newnode* que é tipicamente usada para nós folha e que usa uma estrutura auxiliar pass para guardar informações como linha e coluna do token. A função *newcategory* cria nós com uma categoria específica e é tipicamente usado para statements. A função *newintermediate* cria nós intermédios que são usados para ligar vários nós, estes nós depois não serão impressos na impressão da AST.

Através da gramática é possível construir a AST ao usar a função addchild que adiciona um nó filho a um nó pai, se o nó filho é um intermediate node, os seus filhos são adicionados ao nó pai diretamente.

As tabelas de símbolos usam a estrutura de dados *symbol_list_t*  que é uma lista ligada que possui informações sobre os símbolos e a sua scope. A *global_symbol_table* é inicializada na função *check_program*. As tabelas de símbolos locais são creadas pela função *enter_scope*. Para adicionarmos novos símbolos usa-se *insert_symbol* que retorna NULL se o símbolo já existe. Iniciamos pela função *check_program* que itera pela AST para encontrar variáveis globais e funções. De seguida preenche-se a *global_symbol_table* e processamos cada função recursivamente para preencher a sua tabela de símbolos locais e averiguar a existência de erros semânticos.

De seguida é usada a função *show_symbol_table* para imprimir as tabelas e a função *show_node* que percorre a AST desde a raiz até às suas folhas, anotando-a (caso a flag esteja ativa) e ignorando os intermediate_nodes.

### Geração de Código

Iniciamos pela declaração das funções de *printf()* e *atoi()* da linguagem C. Seguidamente são declaradas as variáveis globais constantes de cadeias de caracteres para cada um dos casos de print: inteiros, floats, boolean (true ou false) e literais de strings. O próximo passo será percorrer a AST do programa e declarar variáveis globais constantes para cada literal de string utilizada, garantindo que não haverá literais declaradas mais do que uma vez. Para isto foi utilizada uma lista ligada de literais, onde é adicionada apenas uma instância da string literal associado a um index, este que é utilizado no nome da variável global constante. É também decalarado uma variável para guardar o endereço dos argumentos de entrada do programa, para que seja possível aceder aos args em qualquer parte do programa. É importante referir que todas as variáveis globais referidas anteriormente que não fazem parte do programa escrito utilizam um "." como prefixo no seu nome, evitando possíveis conflitos com os nomes das variáveis do programa.

Para gerar o código do programa temos de percorrer a tabela de símbolos globais duas vezes. A primeira iteração é feita para declarar apenas todas as variáveis globais presentes no programa. Já a segunda iteração é feita para definir todas as funções presentes no programa. Foi utilizado o prefixo "_" no nome das funções para evitar o conflito entre o main definido no programa e o main do ponto de entrada. No início da função é alocado memória na stack para os parâmetros e para as variáveis locais da função. Os endereços são guardados em registos com o nome do parâmetro/variável, adicionado o prefixo "." no caso dos parâmetros para evitar o conflito com o registo da signature. No caso das branches (if/else, for loops, print de booleans) foi necessário implementar um sistema de criação de labels únicas juntando um tipo e o index, este que é reiniciado para cada função. Todas as funções apresentam um statement de retorno adicionado por defeito, caso não seja explícito no programa.

Por fim, é definido o ponto de entrada *main*. O primeiro passo será guardar os argumentos de entrada na variável global e depois haverá dois casos possíveis. Caso uma função main se encontre na tabela de símbolos global será feita uma chamada a essa função guardando o valor de retorno num registo, devolvendo esse mesmo valor de seguida. Caso não encontre a função main, será apenas retornado o valor 0, possibilitanto a compilação de programas vazios.
