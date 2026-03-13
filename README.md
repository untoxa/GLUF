# GLUF

![Title screen SMS version](/art/screenshots/GLUF_tesla_frog_002.png)

Port of the original ZX Spectrum game by RetroSouls for the [Sega Master System](https://en.wikipedia.org/wiki/Master_System), [Sega Game Gear](https://en.wikipedia.org/wiki/Game_Gear) and [Nintendo Game Boy Color](https://en.wikipedia.org/wiki/Game_Boy_Color). Reimplemented for the [SMS Power’s Competition](https://www.smspower.org/Competitions/Index) 2026 from scratch, using the [CrossZGB](https://github.com/gbdk-2020/CrossZGB) engine.

## Plot

In this game you control a frog named Gluf, and just like the electric car, Gluf can charge energy from places marked with battery symbols. You need the power to activate the circuits in the floor, but you can only charge enough power to activate 10 circuits, so if you need to activate more, then you will need to recharge. When all circuits in the floor are activated the exit door will also be activated, and you will be able to access the next level.

You do not have any kind of weapons so you will need to avoid all the enemies. Certain areas of the levels is only accessible by walking on some unstable floor that will disappear when you walk on it, and this can also be used for making traps for the enemies. For using the elevators you just walk into them and push up or down. While holding A and pressing the direction buttons you can look in those directions for checking out where the monsters are.

You have unlimited lives to complete all 25 levels of the the game.

## In-game screenshots

![In-Game screen SMS version](/art/screenshots/GLUF_tesla_frog_003.png)

## Interesting facts

This game was made using the [CrossZGB](https://github.com/gbdk-2020/CrossZGB) library: the cross-platform (Windows, Linux, MacOS) cross-target (Nintendo Game Boy, Sega Master System and Game Gear) engine for making games. In its turn it is built on top of the [GBDK-2020](https://gbdk.org). The build process is using the GNU Make. A few custom tools were written in Python for processing some of the game assets (level data, metatiles, parallax data), they were integrated into the CrossZGB toolchain through the MakefileCustom file. The game itself is 100% C, all logic is written using the stackful coroutines library which is a part of CrossZGB. So this is probably the only game for the 8-bit platforms so far, which is using cooperative multitasking for everything! The same sources are compiled for all three platforms: Master System, Game Gear, Game Boy Color with very little #ifdef's. I hope this project may serve as a nice tutorial for using the CrossZGB and GBDK-2020, i left a lot of comments all over the code, explaining things.

![Real hardware GG](/art/screenshots/GLUF_on_GG.jpg)

## Credits

Coded by [me](https://github.com/untoxa).

Music and SFX recreated by [Tronimal](https://github.com/Tronimal).

Game Boy Color graphics adaptation by [KirbyKing](https://github.com/KirbyKing186).

Special thanks to [Bbbbbr](https://github.com/bbbbbr), [Chris Maltby](https://github.com/chrismaltby) and everybody who helped with advice and testing.

Original game by [Retrosouls](https://www.retrosouls.net/), written by [Denis Grachev](https://github.com/DenisGrachev). Special thanks and respect for everyone who worked on the design and art of the original game.
