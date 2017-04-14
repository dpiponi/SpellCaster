SpellCaster - a trading card game
=================================

Introduction
============
SpellCaster is a trading card game (think of Magic the Gathering, Pokémon or Yu-Gi-Oh!).
This is an implementation of the game and an AI to play against.
The game is completely playable right now but the implementation needs lots more work.
(For one thing, I need to write the rules and display more status and help information on the screen.)

![SpellCaster screenshot](assets/screenshot.jpg?raw=true "SpellCaster Screenshot")

Technology
==========
The challenge of SpellCaster from a computational perspective is that it is a game of incomplete information - you can't see your opponent's cards or see the order of the cards that haven't yet been drawn. Nonetheless the AI can play well. It beats me anyway. It uses Monte Carlo Tree Search plus some enhancements.

Building
========
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

How to Play
===========
I'll describe how to play at various levels of detail.
We'll start at

Level 0
-------
You lose when you have no more hit points (HP) left.

Level 1
-------
At any moment you and your opponent are also holding a hand of cards. Each player can see their own cards but not their opponents.
Each player also has their own deck of cards from which their hand is replenished.
When it's your turn you pass, discard a card, or use a card to target another acrd or a player.
When a card is targetting it goes into the _Battle Zone_ in the centre of the play field.

![Annotated SpellCaster screenshot](assets/manual.jpg?raw=true "Annotated SpellCaster Screenshot")

When a player passes, the cards in the Battle Zone fight things out and the cards either go to the discard pile or return to their owners.
Along the way, cards may change their properties and players may lose hit points.
When a player loses all of their hit points they lose.
