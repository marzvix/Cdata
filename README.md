* qua, 18 de dez de 2019 23:12:48

Ok, tudo migrado e compilado.

Observações:

1. O código do livro é mais moderno e enxuto
   (apeser da minha codebase estar bugada)
2. Usei chaves de compilação para minimizar os problemas
   basicamente -x c (compila codigo c em vez de cpp)
   e o controle com o define
3. A tela apresentou o mesmo problema que eu havia notado
   no iniício: o sistema tem que ser ajustado e eu quero
   evitar o curses nessa etapa, portanto vou
   reaproveitar o código de testes com o tty que fiz
   e essa vai ser a primeira alteração da codebase original

* qua, 18 de dez de 2019 15:02:52

Merge do master com a codebase CDATA.ZIP

Obs:

- Preparativos para gerar e compilar com o gnu c.
- Ainda não deve compilar: vai requerer os ajustes nos fontes


qua, 18 de dez de 2019 14:43:58

Alguns ajustes:

1. o programa deve ser compilado com o TCDBMS.BAT: o make do
   livro é incompatível com os códigos do CDATA.ZIP

2. os códigos em c estão todos com armaduras para vários
   compiladores dos (isso vai me indicar onde tenho que
   alterar para compilar com o gnu c)

3. o formato de geração do schema é diferente do que foi
   explicado no livro. O autor usa piping para passar
   os arquivo de schema e gera tres arquivos .c1, .c2 e .c3
   que são usados no código, isso vai requerer comparações
   com a codebase dos

4. Esse é um branch terminal, vou transferir-me para o master
   e fazer as cópias dos arquivo sobre a codebase do cygwin
   e continuar no ambiente cygwin que é mais confortável
   do que o do dosbox.
5. Quanto tiver funcionado, no cygwin retorno ajustando essa
   codebase e faço um merge com as armaduras para todos
   os compiladores (dos, linux, windows, etc)

* qua, 18 de dez de 2019 14:00:43

Fiz o merge final.
Agora sigo daqui com os fontes originais e corretos para
conversao e testes com o cygwin.


* Qua, 18 de dez de 2019 13:14:28

Este BRANCH DOS foi criado hoje, mas estava sem controle
do git desde do ultimo dia 05.

Nele eu fiz a migração do código que eu estava tentando
implementar - no ambiente do cygwin - para o ambiente nativo
do DOS.

Fiz uma instalação completa do Dosbox e incluí as ferramentas

- Emacs
- Borland C
- e outras.

Mas esse código contém muitos erros pois foi digitado, copiando
as fontes do Livro: C Database Development, Al Stevens.

Esse livro foi pego emprestado na biblioteca: Internet Archive

no link: https://archive.org/details/cdatabasedevelop00stev
No contratempo da digitação, acabei por encontrar os fontes
que eram vendidos separadamente num disquete, conforme anúncio
do próprio livro.

Encontrei os arquivos em: Programer's Corner (https://www.pcorner.com)

https://www.pcorner.com/download?filename=3zKgSP0ww36E8v4QcJl1AT2bMW2YgpemoybtzSDItpQ%3D
