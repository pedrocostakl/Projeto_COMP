# deiGo - COMP2024

Autores:
- Marco Manuel Almeida e Silva - 2021211653
- Pedro Sousa da Costa - 2022220304

### Gramática

400 palavras.

### Estrututas de Dados (AST e tabela de símbolos)

400 palavras.

### Geração de Código

Iniciamos pela declaração das funções de *printf()* e *atoi()* da linguagem C. Seguidamente são declaradas as variáveis globais constantes de cadeias de caracteres para cada um dos casos de print: inteiros, floats, boolean (true ou false) e literais de strings. O próximo passo será percorrer a AST do programa e declarar variáveis globais constantes para cada literal de string utilizada, garantindo que não haverá literais declaradas mais do que uma vez. O nome da variável será uma hash correspondente à cadeia de caracteres que será guardada no node da AST para que seja possível verificar se a hash já foi utilizada (ou seja, se já foi declarada). É também decalarado uma variável para guardar o endereço dos argumentos de entrada do programa, para que seja possível aceder aos args em qualquer parte do programa. É importante referir que todas as variáveis globais referidas anteriormente que não fazem parte do programa escrito utilizam um "." como prefixo no seu nome, evitando possíveis conflitos com os nomes das variáveis do programa.

Para gerar o código do programa temos de percorrer a tabela de símbolos globais duas vezes. A primeira iteração é feita para declarar apenas todas as variáveis globais presentes no programa. Já a segunda iteração é feita para definir todas as funções presentes no programa. Foi utilizado o prefixo "_" no nome das funções para evitar o conflito entre o main definido no programa e o main do ponto de entrada. No início da função é alocado memória na stack para os parâmetros e para as variáveis locais da função. Os endereços são guardados em registos com o nome do parâmetro/variável, adicionado o prefixo "." no caso dos parâmetros para evitar o conflito com o registo da signature. No caso das branches (if/else, for loops, print de booleans) foi necessário implementar um sistema de criação de labels únicas juntando um tipo e o index, este que é reiniciado para cada função. Todas as funções apresentam um statement de retorno adicionado por defeito, caso não seja explícito no programa.

Por fim, é definido o ponto de entrada *main*. O primeiro passo será guardar os argumentos de entrada na variável global e depois haverá dois casos possíveis. Caso uma função main se encontre na tabela de símbolos global será feita uma chamada a essa função guardando o valor de retorno num registo, devolvendo esse mesmo valor de seguida. Caso não encontre a função main, será apenas retornado o valor 0, possibilitanto a compilação de programas vazios.
