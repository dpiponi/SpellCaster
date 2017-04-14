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
At any moment you and your opponent are also holding a hand of cards. Each player can see their own cards but not their opponent's.
Each player also has their own deck of cards from which their hand is replenished.
When it's your turn you pass, discard a card, or use a card to target another card or a player.
When a card is targetting it goes into the _Battle Zone_ in the centre of the play field.

![Annotated SpellCaster screenshot](assets/manual.jpg?raw=true "Annotated SpellCaster Screenshot")

When a player passes, the cards in the Battle Zone fight things out and the cards either go to the discard pile or return to their owners.
Along the way, cards may change their properties and players may lose hit points.
When a player loses all of their hit points they lose.

Level 2
-------
Before the game starts, each player has a deck of SpellCaster cards obtained from wherever.
Each player shuffles the other player's deck and places it face down.
At the start of the game, each player draws eight cards from the top of their own deck to form their hand.
Each player can see only their own hand, although cards in the other player's hand may become exposed during play.
In the computer game, the player's cards are arranged at the bottom and the computer's cards are along the top.
The computer's cards are generally shown face down, but any that have been exposed at any point during the game are shown face up.

A player's turn consists of one of:
1. Discarding a card. A player may click on any card in their hand and then click on *DISCARD*. It will go into the discard pile.
   As soon as the card is clicked on, the *DISCARD* button will be highlighted showing it is a valid option.
2. Playing a card. Cards suitable for playing are highlighted in green. There may not be any suitable cards.
   Once the chosen card is clicked on the valid targets for that card are highlighted. The player now selects a target.
   Valid targets can be cards already in the Battle Zone or players.
   The immediate effect of the play is executed.
   Usually this means the card moves to the right of the Battle Zone where it is annotated by lines showing its target.
   Some cards have an immediate effect, for example giving _mana_ to a player. (More on mana later.)
   are highlighted.
3. Passing. This triggers a _Battle_. Each card in the Battle Zone is executed in turn working from right to left.
   For cards other than straightforward monsters, the execution process for each card is written on the card itself.
   Once the entire Battle is played out, each player draws enough cards to bring the size of their hand up to 8.
   If a player has more than 8 cards they don't draw, but they don't have to discard either.
Play then continues with the other player.

As mentioned above, only certain cards can be played to target other cards.
The first constrain is mana.
There are two kinds of mana: red and blue.
They correspond to worldly and astral magic respectively.
The cost to cast a spell is written on each card and the mana available to each player is displayed alongside each player icon.
Cards come in three classes:
1. Monsters. These typically execute by doing damage to another monster or another player.
   The phrase "Doing damage" is another way of saying they remove hit points.
   Some monsters may have other effects. For example a gelatinous cube can temporarily immobilise its target by paralysing it.
   If a monster's hit points are reduced to zero it is killed and goes to the discard pile.
   Otherwise it returns to its owner's hand.
   (Monsters can change ownership during a Battle so it might not return to the hand it came from.)
2. Spells. These have a variety of effects. Usually at the end of its execution a spell goes to the discard pile.
   Spells include the Mana cards.
   These usually have zero cost and immediately give the target player the amount of Mana indicated on the card.
3. Artifacts. These have effects similar to spells execept they return to their owner's hand after play.
Each card can only target certain classes of card. The type of card it can target can be viewed by hovering over the card with the mouse.
Hovering causes detailed information about the card to be displayed at the left.

In addition to being members of classes, there are properties cards can have, for example they can be _undead_, or _fire resistant_, or _blessed_.
Cards may have some requirements that need to be satisfied before they can target another card.
For example _Holy Water_ can only target undead monsters.
In addition, some cards have exclusions.
For example _Ant Swarm_ cards can't (usually) target flying monsters.
Cards can change the properties of other cards.
For example the _Flight_ spell can make any other card fly. So the _Ant Swarm_ mentioned aboce could attack a flying _Dragon_ once it has _Flight_ cast on it.

More to come...
