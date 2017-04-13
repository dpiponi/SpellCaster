SpellCaster - a trading card game
---------------------------------

Introduction
------------
SpellCaster is a trading card game (think of Magic the Gathering, Pokémon or Yu-Gi-Oh!).
This is an implementation of the game and an AI to play against.
The game is completely playable right now but the implementation needs lots more work.
(For one thing, I need to write the rules and display more status and help information on the screen.)

![SpellCaster screenshot](assets/screenshot.jpg?raw=true "SpellCaster Screenshot")

Technology
----------
The challenge of SpellCaster from a computational perspective is that it is a game of incomplete information - you can't see your opponent's cards or see the order of the cards that haven't yet been drawn. Nonetheless the AI can play well. It beats me anyway. It uses Monte Carlo Tree Search plus some enhancements.

Building
--------
To build SpellCaster requires cmake 3.1 or later.
You also need to have installed SDL2, SDL2_image and Freetype.
It requires a C++14 compiler such as clang.

To build it use:

    git clone https://github.com/dpiponi/SpellCaster.git
    cd SpellCaster
    cmake .
    make

To run it use:

    ./sc

The files `player.txt` and `computer.txt` specify the decks used by you and the computer.
Have a look in `test.json` for things you can tweak.
You may need to modify the "key" entry to point to a .?tf font that actually exists on your machine.
