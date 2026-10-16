# Trabalho M2 - Computação Gráfica

## Integrantes

Felipe Skiavine Hirata  
Gustavo Zabot Segeuca  
Lucas Toller Gutmann  
Marco Antonio Bigolin Zanandrea  

Professor: Rodrigo Lyra  
Disciplina: Computação Gráfica

Projeto da M2 organizado em módulos simples, utilizando os exemplos fornecidos pelo professor como referência para carregamento de OBJ, iluminação e texturas.

## Estrutura

```text
Trabalho_M2_Computacao_Grafica/
├── main.cpp             # janela, transformações, luzes e teclado
├── obj_loader.h
├── obj_loader.cpp       # leitura do OBJ, triangulação, normais e desenho
├── textura.h
├── textura.cpp          # leitura de BMP 24 bits e criação da textura OpenGL
├── data/                # modelos .obj
├── Textures/            # imagens BMP fornecidas pelo professor
├── freeglut/
├── freeglut.dll
└── .gitignore
```

## Requisitos implementados

- leitura de `v`, `vt`, `vn` e `f` de arquivos OBJ;
- formatos de face `v`, `v/vt`, `v//vn` e `v/vt/vn`;
- índices positivos e negativos do padrão OBJ;
- faces com 3 ou mais pontos;
- triangulação em leque e renderização com `GL_TRIANGLES`;
- uso das normais do OBJ e cálculo de normal de face quando necessário;
- leitura de BMP 24 bits e aplicação das coordenadas `vt`;
- material com componentes ambiente, difusa e especular;
- três pontos de luz com ambiente, difusa, especular e posição;
- liga/desliga individual das três luzes;
- translação com `glTranslatef`;
- rotação com `glRotatef`;
- escala com `glScalef`;
- perspectiva, depth buffer e `GL_NORMALIZE`;
- centralização e escala inicial automática do modelo.

## Modelo principal

O programa carrega:

```text
data/mba1.obj
```

Esse modelo possui `v`, `vt`, `vn` e `f`, permitindo demonstrar todos os itens do trabalho.

## Textura

É carregada a imagem:

```text
Textures/launch.bmp
```

A textura começa **desligada**. O arquivo `.mtl` original do `mba1.obj` não foi fornecido, então o BMP serve para demonstrar o mapeamento das coordenadas `vt`. Pressione `T` para ligar/desligar.

## Observação sobre a textura

O modelo `mba1.obj` contém coordenadas de textura (`vt`), porém o
arquivo de material/textura original referenciado pelo modelo não foi
disponibilizado. Por isso foi utilizada a textura `launch.bmp`,
fornecida nos exemplos da disciplina, para demonstrar o carregamento e
a aplicação das coordenadas de textura.

## Controles

| Tecla | Função |
|---|---|
| ↑ / ↓ | Translação no eixo Y |
| ← / → | Translação no eixo X |
| Q / E | Translação no eixo Z |
| + / - | Aumentar / diminuir escala |
| x / X | Rotação positiva / negativa em X |
| y / Y | Rotação positiva / negativa em Y |
| z / Z | Rotação positiva / negativa em Z |
| 1 | Liga/desliga luz 1 |
| 2 | Liga/desliga luz 2 |
| 3 | Liga/desliga luz 3 |
| T | Liga/desliga textura |
| R | Reinicia transformações |
| ESC | Encerra |

## Compilar no VS Code / MinGW

Na pasta raiz do projeto:

```powershell
g++ main.cpp obj_loader.cpp textura.cpp -Ifreeglut/include -Lfreeglut/lib -lfreeglut -lopengl32 -lglu32 -o trabalho_m2.exe
```

Se necessário, copie a DLL:

```powershell
Copy-Item ".\freeglut\bin\freeglut.dll" ".\freeglut.dll"
```

Depois execute:

```powershell
.\trabalho_m2.exe
```

## Organização para a defesa

- `main.cpp`: fluxo geral, três luzes, material, transformações e eventos de teclado;
- `obj_loader.cpp`: leitura `v/vt/vn/f`, triangulação e `GL_TRIANGLES`;
- `textura.cpp`: leitura BMP e criação da textura OpenGL.