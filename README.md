cpp-oxygine-chess
A fully functional 2D chess game built with the Oxygine game engine. The game features a complete chess implementation with move validation, check/checkmate detection, castling, pawn promotion, and a clean graphical interface.

🎮 Features

Complete chess rules implementation (excluding en passant)

Turn-based gameplay with visual turn indication

Piece movement validation for all chess pieces:

Pawns (including double first move)

Rooks, Knights, Bishops

Queens and Kings

Special moves:

Castling (both kingside and queenside)

Pawn promotion to Queen

Game state detection:

Check detection

Checkmate detection

Stalemate detection

Visual feedback:

Selected piece highlighting (separate textures)

Victory screens ("White Wins", "Black Wins", "Draw")

Smooth piece animations using Tween system

Main menu with Play and Exit buttons

Resource management via XML atlas configuration

🖥️ Gameplay

Left-click to select a piece (only pieces of the current player's color can be selected)

Left-click on a valid destination square to move the selected piece

Invalid moves are ignored, and the piece remains selected

The game automatically detects check, checkmate, and stalemate situations

🛠️ Technologies Used

C++17 as the primary programming language

Oxygine Framework for 2D graphics and game engine functionality

CMake as the build system

SDL2 for cross-platform windowing and input (via Oxygine)

🔧 Building and Running

Prerequisites

CMake (version 3.10 or higher)

C++17 compatible compiler (GCC 7+, Clang 5+)

SDL2 development libraries

Oxygine Framework (included as submodule)

Build Instructions (Linux)

Clone the repository with submodules:

bash

git clone --recursive https://github.com/yourusername/cpp-oxygine-chess.git

cd cpp-oxygine-chess

Install dependencies (Ubuntu/Debian):

bash

sudo apt-get update

sudo apt-get install cmake build-essential libsdl2-dev

Build and run using the provided script:

bash

cd proj.cmake

chmod +x run.sh

./run.sh

Manual Build

bash

mkdir -p build

cd build

cmake ..

make

cp -r ../data/* ./

./Chess
